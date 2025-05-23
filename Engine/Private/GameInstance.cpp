#include "..\Public\GameInstance.h"

#include "Layer.h"

#include "MyCamera.h"

#include "Light.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{

}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, OUT ID3D11Device** ppDevice, OUT ID3D11DeviceContext** ppContext)
{
	m_pGraphic_Device = CGraphic_Device::Create(GraphicDesc, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pKey_Manager = CKey_Manager::Create();
	if (nullptr == m_pKey_Manager)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pCollision_Manager = CCollision_Manager::Create();
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pSound_Manager = CSound_Manager::Create();
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pEvent_Manager = CEvent_Manager::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pCalculateTransform = CTransform::Create(*ppDevice, *ppContext);
	if (nullptr == m_pCalculateTransform)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	Update_TimeScaleTimer(fTimeDelta);

	m_pObject_Manager->Manage_LifeCycle();

	m_pKey_Manager->Update();

	m_pEvent_Manager->Process_Events();

	m_pObject_Manager->PriorityTick(fTimeDelta * m_fTimeScale);
	m_pObject_Manager->Tick(fTimeDelta * m_fTimeScale);

	if (nullptr != m_pMain_Camera && m_pMain_Camera->Is_Active())
		m_pMain_Camera->Tick(fTimeDelta);

	m_pCollision_Manager->Update();

	if (nullptr != m_pMain_Camera && m_pMain_Camera->Is_Active())
		m_pMain_Camera->LateTick(fTimeDelta * m_fTimeScale);

	m_pPipeLine->Update();
	m_pFrustum->Update();

	m_pObject_Manager->LateTick(fTimeDelta * m_fTimeScale);

	m_pRenderer->Tick(fTimeDelta * m_fTimeScale);
	m_pLevel_Manager->Tick(fTimeDelta);
	m_pSound_Manager->Tick(fTimeDelta);
}

HRESULT CGameInstance::Draw()
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pRenderer)
		return E_FAIL;

	m_pRenderer->Draw();

	return m_pLevel_Manager->Render();
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	m_pRenderer->Clear();
	m_pObject_Manager->Clear(iLevelIndex);	
	m_pComponent_Manager->Clear(iLevelIndex);

	if (1 != iLevelIndex)
	{
		m_pCollision_Manager->Clear();
	}
}

void CGameInstance::Clear_Level(_uint iLevelIndex)
{
	m_pRenderer->Clear();
	m_pObject_Manager->Clear(iLevelIndex);
}

#pragma region GRAPHIC_DEVICE


HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

#pragma endregion 

#pragma region LEVEL_MANAGER
void CGameInstance::Change_Level(CLevel * pNewLevel)
{
	m_pLevel_Manager->Change_Level(pNewLevel);
}

_uint CGameInstance::Get_CurrentLevelID()
{
	return m_pLevel_Manager->Get_CurrentLevelID();
}
void CGameInstance::Set_CurrentLevelID(_uint iLevelID)
{
	m_pLevel_Manager->Set_CurrentLevelID(iLevelID);
}
#pragma endregion

#pragma region TIMER_MANAGER
_float CGameInstance::Get_TimeDelta(const wstring & strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}
HRESULT CGameInstance::Add_Timer(const wstring & strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimerTag);
}
void CGameInstance::Compute_TimeDelta(const wstring & strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return ;

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

void CGameInstance::Set_TimeScale(_float fTimeScale)
{
	m_fTimeScale = fTimeScale;
}
void CGameInstance::Set_TimeScaleWithRealTime(_float fTimeScale, _float fRealTime)
{
	m_fTimeScale = fTimeScale;
	m_fTimerForTimeScale = fRealTime;
}

void CGameInstance::Update_TimeScaleTimer(_float fTimeDelta)
{
	if (m_fTimerForTimeScale > 0.f)
	{
		m_fTimerForTimeScale -= fTimeDelta;
		if (m_fTimerForTimeScale <= 0.f)
		{
			m_fTimerForTimeScale = 0.f;
			m_fTimeScale = 1.f;
		}
	}
}
#pragma endregion

#pragma region OBJECT_MANAGER
HRESULT CGameInstance::Add_Prototype(const wstring & strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
	
}
CGameObject* CGameInstance::Add_Clone(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Add_Clone(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CGameObject* CGameInstance::Clone_GameObject(const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(strPrototypeTag, pArg);
}

CComponent* CGameInstance::Find_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
}

CGameObject* CGameInstance::Find_GameObject(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_GameObject(iLevelIndex, strLayerTag, iIndex);
}

CGameObject* CGameInstance::Find_Prototype(const wstring& strPrototypeTag)
{
	return m_pObject_Manager->Find_Prototype(strPrototypeTag);
}

CLayer* CGameInstance::Find_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_Layer(iLevelIndex, strLayerTag);
}

void CGameInstance::Clear_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{

}

void CGameInstance::Insert_GameObject(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pObj)
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Insert_GameObject(iLevelIndex, strLayerTag, pObj);
}


#pragma region COMPONENT_MANAGER
HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;
	
	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}
CComponent* CGameInstance::Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag)
{
	return m_pComponent_Manager->Find_Prototype(iLevelIndex, strPrototypeTag);
}
#pragma endregion


#pragma region RENDERER
HRESULT CGameInstance::Add_RenderObject(CRenderer::RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderObject(eRenderGroup, pRenderObject);
}

HRESULT CGameInstance::Add_RenderComponent(CComponent* pRenderComponent)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderComponent(pRenderComponent);
}
void CGameInstance::Add_UsingShader(CShader* pShader)
{
	m_pRenderer->Add_UsingShader(pShader);
}
void CGameInstance::Active_RadialBlur(const RADIALBLUR_DESCS& Descs)
{
	m_pRenderer->Active_RadialBlur(Descs);
}
void CGameInstance::Inactive_RadialBlur(_uint iActorID, _float fLerpTime)
{
	m_pRenderer->Inactive_RadialBlur(iActorID, fLerpTime);
}
void CGameInstance::Update_BlurCenterWorld(_vector vBlurCenterWorld)
{
	m_pRenderer->Update_BlurCenterWorld(vBlurCenterWorld);
}


#pragma endregion

#pragma region COLLISION_MANAGER

void CGameInstance::Add_ColliderToLayer(const string& strLayer, CCollider* pCollider)
{
	if (nullptr == m_pCollision_Manager)
		return;

	m_pCollision_Manager->Add_ColliderToLayer(strLayer, pCollider);
}

#pragma endregion


#pragma region SOUND_MANAGER

HRESULT CGameInstance::Create_Sound(const string& strPath, const wstring& strSoundTag)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Create_Sound(strPath, strSoundTag);
}

void CGameInstance::Play(const wstring& strSoundTag, _bool bLoop, _float fVolume)
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->Play(strSoundTag, bLoop, fVolume);

}

void CGameInstance::Play_RandomSound(const wstring& strSoundTag, _int iMin, _int iMax, _bool bLoop, _float fVolume)
{
	if (nullptr == m_pSound_Manager)
		return;

	std::random_device RD;
	std::mt19937 gen(RD());

	std::uniform_int_distribution<_int> dis(iMin, iMax);

	m_pSound_Manager->Play(strSoundTag + to_wstring(dis(gen)), bLoop, fVolume);
}

void CGameInstance::Stop(const wstring& strSoundTag)
{
	if (nullptr == m_pSound_Manager)
		return;
	m_pSound_Manager->Stop(strSoundTag);
}


void CGameInstance::SetVolume(const wstring& strSoundTag, const _float& fVolume)
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->SetVolume(strSoundTag, fVolume);
}

void CGameInstance::SetPosition(const wstring& strSoundTag, _float fPosition)
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->SetPosition(strSoundTag, fPosition);
}


_bool CGameInstance::Is_Playing(const wstring& strSoundTag)
{
	if (nullptr == m_pSound_Manager)
		return false;

	return m_pSound_Manager->Is_Playing(strSoundTag);
}
void CGameInstance::Set_Sound_FadeOut(const wstring& strSoundTag, _float fTime)
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->Set_Sound_FadeOut(strSoundTag, fTime);
}
CSound* CGameInstance::Get_Sound(const wstring& strSoundTag)
{
	if (nullptr == m_pSound_Manager)
		return nullptr;

	return m_pSound_Manager->Get_Sound(strSoundTag);
}
#pragma endregion


#pragma region FRUSTUM
_bool CGameInstance::In_WorldFrustum(_fvector vWorldPos, _float fRadius)
{
	return m_pFrustum->In_WorldFrustum(vWorldPos, fRadius);
}
#pragma endregion


#pragma region PIPELINE

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eTransformState, TransformMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix(eTransformState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eTransformState);
}
_float4x4 CGameInstance::Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4_TP(eTransformState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4_Inverse_TP(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4_Inverse_TP(eTransformState);
}

_float4 CGameInstance::Get_CamPosition() const
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamPosition();
}
#pragma endregion


#pragma region EVENT_MANAGER
void CGameInstance::Add_Event(function<void()> pFunc)
{
	if (nullptr == m_pEvent_Manager)
		return;

	m_pEvent_Manager->Add_Event(move(pFunc));
}
#pragma endregion


#pragma region FONT_MANAGER
HRESULT CGameInstance::Add_Font(const wstring& strFontTag, const wstring& strFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, strFontFilePath);
}

HRESULT CGameInstance::Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _float fScale, _fvector vColor, _float fRotation)
{
	return m_pFont_Manager->Render(strFontTag, strText, vPosition, vColor, fRotation, fScale);
}
#pragma endregion

#pragma region TARGET_MANAGER

HRESULT CGameInstance::Add_RenderTarget(const wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixel, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strRenderTargetTag, iWidth, iHeight, ePixel, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const wstring& strMRTTag, const wstring& strRenderTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strRenderTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const wstring& strMRTTag)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag);
}

HRESULT CGameInstance::Begin_MRT_NoClear(const wstring& strMRTTag)
{
	return m_pTarget_Manager->Begin_MRT_NoClear(strMRTTag);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RT_SRV(const wstring& strRenderTargetTag, CShader* pShader, const _char* pConstantName)
{
	return m_pTarget_Manager->Bind_RT_SRV(strRenderTargetTag, pShader, pConstantName);
}

HRESULT CGameInstance::Clear_Target(const wstring& strTargetTag)
{
	return m_pTarget_Manager->Clear(strTargetTag);
}

#ifdef _DEBUG

HRESULT CGameInstance::Ready_RTDebug(const wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strRenderTargetTag, fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Render_RTDebug(const wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render_Debug(strMRTTag, pShader, pVIBuffer);
}

HRESULT CGameInstance::Render_SingleRTDebug(const wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render_SingleTargetDebug(strMRTTag, pShader, pVIBuffer);
}

#endif

#pragma endregion

#pragma region LIGHT_MANAGER

const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex) const
{
	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(CLight* pLight)
{
	return m_pLight_Manager->Add_Light(pLight);
}

HRESULT CGameInstance::Render_Lights(CShader* pShader, CVIBuffer* pVIBuffer)
{
	return m_pLight_Manager->Render(pShader, pVIBuffer);
}

#pragma endregion


#pragma region CAMERA

void CGameInstance::Change_MainCamera(CCamera* pCamera)
{
	Safe_Release(m_pMain_Camera);

	m_pMain_Camera = pCamera;
}

void CGameInstance::Update_ViewProj()
{
	if (nullptr != m_pMain_Camera)
		m_pMain_Camera->Update_ViewProj();
}

#pragma endregion


void CGameInstance::Release_Engine()
{
	CModel::Release_Textures();

	CGameInstance::Get_Instance()->Free();

	CGameInstance::Get_Instance()->Destroy_Instance();	
}

void CGameInstance::Free()
{	
	Safe_Release(m_pMain_Camera);
	Safe_Release(m_pCalculateTransform);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pEvent_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pGraphic_Device);
}
