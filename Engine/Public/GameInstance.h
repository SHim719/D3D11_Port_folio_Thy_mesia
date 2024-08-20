#pragma once

#include "Manager_Headers.h"

/* 클라이언트와 엔진의 중계자의 역활을 수행한다. */
/* 클라이언트에서 엔진의 기능을 쓰고하자 한다라면 무조건 게임인스턴스를 떠올리면 되도록. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(_uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, OUT ID3D11Device** ppDevice, OUT ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);
	HRESULT	Draw();
	void Clear(_uint iLevelIndex);
	void Clear_Level(_uint iLevelIndex);

#pragma region GRAPHIC_DEVICE
public:
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();
#pragma endregion

#pragma region LEVEL_MANAGER
public: /* For.Level_Manager */
	void Change_Level(class CLevel* pNewLevel);
	_uint Get_CurrentLevelID();
	void Set_CurrentLevelID(_uint iLevelID);
#pragma endregion

#pragma region TIMER_MANAGER
public:
	_float	Get_TimeDelta(const wstring& strTimerTag);
	HRESULT Add_Timer(const wstring& strTimerTag);
	void Compute_TimeDelta(const wstring& strTimerTag);
	void Set_TimeScale(_float fTimeScale);
	_float Get_TimeScale() { return m_fTimeScale; }

	void Set_TimeScaleWithRealTime(_float fTimeScale, _float fRealTime);
	void Update_TimeScaleTimer(_float fTimeDelta);
private:
	_float		m_fTimeScale = 1.f;
	_float		m_fTimerForTimeScale = { 0.f };
#pragma endregion

#pragma region OBJECT_MANAGER
public:
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	class CGameObject* Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	class CGameObject* Clone_GameObject(const wstring& strPrototypeTag, void* pArg = nullptr);
	class CComponent* Find_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex = 0);
	class CGameObject* Find_GameObject(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex = 0);
	class CGameObject* Find_Prototype(const wstring& strPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);
	void Clear_Layer(_uint iLevelIndex, const wstring& strLayerTag);

	void Insert_GameObject(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pObj);
#pragma endregion

#pragma region COMPONENT_MANAGER
public:
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);

	class CComponent* Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);
#pragma endregion


#pragma region RENDERER
public:
	HRESULT Add_RenderObject(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Add_RenderComponent(CComponent* pRenderComponent);
	void Add_UsingShader(class CShader* pShader);

	void Active_RadialBlur(const RADIALBLUR_DESCS& Descs);
	void Inactive_RadialBlur(_float fLerpTime);
	void Update_BlurCenterWorld(_vector vBlurCenterWorld);
	_bool Is_Active_RadialBlur() const {
		return m_pRenderer->Is_Active_RadialBlur();
	}

	void Active_ColorInversion() {
		m_pRenderer->Active_ColorInversion();
	}
	void Inactive_ColorInversion(_float fLerpTime) {
		m_pRenderer->Inactive_ColorInversion(fLerpTime);
	}

#pragma endregion

#pragma region KEY_MANAGER
	bool GetKeyDown(eKeyCode _keyCode) { return	 m_pKey_Manager->GetKeyDown(_keyCode); }
	bool GetKeyUp(eKeyCode _keyCode)	{ return m_pKey_Manager->GetKeyUp(_keyCode); }
	bool GetKey(eKeyCode _keyCode)	{ return m_pKey_Manager->GetKey(_keyCode); }
	bool GetKeyNone(eKeyCode _keyCode) { return m_pKey_Manager->GetKeyNone(_keyCode); }
#pragma endregion


#pragma region COLLISION_MANAGER
	void Add_ColliderToLayer(const string& strLayer, CCollider* pCollider);
#pragma endregion

#pragma region SOUND_MANAGER
	HRESULT Create_Sound(const string& strPath, const wstring& strSoundTag);
	void	Play(const wstring& strSoundTag, _bool bLoop, _float fVolume = 1.f);
	void	Play_RandomSound(const wstring& strSoundTag, _int iMin, _int iMax, _bool bLoop, _float fVolume = 1.f);
	void	Stop(const wstring& strSoundTag);
	void	SetVolume(const wstring& strSoundTag, const _float& fVolume);
	void	SetPosition(const wstring& strSoundTag, _float fPosition);
	_bool	Is_Playing(const wstring& strSoundTag);
	void	Set_Sound_FadeOut(const wstring& strSoundTag, _float fTime);
	class CSound* Get_Sound(const wstring& strSoundTag);
#pragma endregion

#pragma region FRUSTUM
	_bool In_WorldFrustum(_fvector vWorldPos, _float fRadius);
#pragma endregion 

#pragma region PIPELINE
	void Set_Transform(CPipeLine::TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix);
	_matrix Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4x4 Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4x4 Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4x4 Get_TransformFloat4x4_Inverse_TP(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4 Get_CamPosition() const;
#pragma endregion


#pragma region EVENT_MANAGER
	void Add_Event(function<void()> pFunc);
#pragma endregion

#pragma region FONT_MANAGER
	HRESULT Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _float fScale = 1.f, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRotation = 0.f);
#pragma endregion

#pragma region TARGET_MANAGER
public:
	HRESULT Add_RenderTarget(const wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixel, const _float4& vClearColor);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strRenderTargetTag);
	HRESULT Begin_MRT(const wstring& strMRTTag);
	HRESULT Begin_MRT_NoClear(const wstring& strMRTTag);
	HRESULT End_MRT();
	HRESULT Bind_RT_SRV(const wstring& strRenderTargetTag, class CShader* pShader, const _char* pConstantName);
	HRESULT Clear_Target(const wstring& strTargetTag);

#ifdef _DEBUG
	HRESULT Ready_RTDebug(const wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_RTDebug(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Render_SingleRTDebug(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif
#pragma endregion

#pragma region LIGHT_MANAGER
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	HRESULT Add_Light(class CLight* pLight);
	HRESULT Render_Lights(class CShader* pShader, CVIBuffer* pVIBuffer);
#pragma endregion

#pragma region CAMERA
	class CCamera* m_pMain_Camera = { nullptr };

	void Change_MainCamera(class CCamera* pCamera);
	void Update_ViewProj();

	class CCamera* Get_MainCamera() {
		return m_pMain_Camera;
	}
#pragma endregion

#pragma region TRANSFORM
	// 행렬 계산을 위한 Transform
	CTransform* m_pCalculateTransform = { nullptr };

	CTransform* Get_CalculateTF() {
		return m_pCalculateTransform;
	}
#pragma endregion 


private:
	CGraphic_Device*		m_pGraphic_Device = { nullptr };
	CLevel_Manager*			m_pLevel_Manager = { nullptr };
	CTimer_Manager*			m_pTimer_Manager = { nullptr };
	CObject_Manager*		m_pObject_Manager = { nullptr };
	CComponent_Manager*		m_pComponent_Manager = { nullptr };
	CKey_Manager*			m_pKey_Manager = { nullptr };
	CCollision_Manager*		m_pCollision_Manager = { nullptr };
	CSound_Manager*			m_pSound_Manager = { nullptr };
	CFrustum*				m_pFrustum = { nullptr };
	CRenderer*				m_pRenderer = { nullptr };
	CPipeLine*				m_pPipeLine = { nullptr };
	CEvent_Manager*			m_pEvent_Manager = { nullptr };
	CFont_Manager*			m_pFont_Manager = { nullptr };
	CTarget_Manager*		m_pTarget_Manager = { nullptr };
	CLight_Manager*			m_pLight_Manager = { nullptr };
	
public:
	static void Release_Engine();
	virtual void Free() override;
	
};

END