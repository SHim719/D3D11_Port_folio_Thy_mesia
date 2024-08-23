#include "..\Public\Renderer.h"
#include "GameObject.h"

#include "GameInstance.h"



CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}


HRESULT CRenderer::Initialize()
{
	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewports = { 1 };
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_OriginViewPort = ViewportDesc;

	if (FAILED(Ready_DefaultTargets((_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height)))
		return E_FAIL;

	if (FAILED(Ready_LightTargets((_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height)))
		return E_FAIL;

	if (FAILED(Ready_DeferredTarget((_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height)))
		return E_FAIL;

	if (FAILED(Ready_StencilTarget((_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height)))
		return E_FAIL;

	if (FAILED(Ready_EffectTargets((_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height)))
		return E_FAIL;

	if (FAILED(Ready_GlowTargets((_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height)))
		return E_FAIL;
	
	if (FAILED(Ready_BloomTargets((_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height)))
		return E_FAIL;

	if (FAILED(Ready_Buffers()))
		return E_FAIL;

	if (FAILED(Ready_Shaders()))
		return E_FAIL;


	if (FAILED(Bind_Matrices()))
		return E_FAIL;


#ifdef _DEBUG
	
	if (FAILED(Ready_Debug()))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CRenderer::Add_RenderObject(RENDERGROUP eRenderGroup, CGameObject * pRenderObject)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].emplace_back(pRenderObject);

	Safe_AddRef(pRenderObject);	

	return S_OK;
}

HRESULT CRenderer::Add_RenderComponent(CComponent* pRenderComponent)
{
	if (nullptr == pRenderComponent)
		return E_FAIL;

	m_RenderComponents.emplace_back(pRenderComponent);

	Safe_AddRef(pRenderComponent);

	return S_OK;
}

void CRenderer::Add_UsingShader(CShader* pShader)
{
	Safe_AddRef(pShader);
	m_UsingShaders.emplace_back(pShader);
}

void CRenderer::Tick(_float fTimeDelta)
{
	if (m_bRadialBlur)
		Update_RadialBlur(fTimeDelta);

	if (m_bColor_Inversion)
		Update_Color_Inversion(fTimeDelta);
}

HRESULT CRenderer::Draw()
{
	m_pGameInstance->Update_ViewProj();
	Bind_ViewProj();

	if (FAILED(Render_NonBlend()))
		return E_FAIL;

	if (FAILED(Render_LightAcc()))
		return E_FAIL;

	if (FAILED(Render_Deferred()))
		return E_FAIL;

	if (FAILED(Render_Effect()))
		return E_FAIL;

	if (FAILED(Render_Glow()))
		return E_FAIL;

	if (FAILED(Render_Distortion()))
		return E_FAIL;

	if (FAILED(Render_Stencil()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;

	if (FAILED(Render_PostProcess()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;


#ifdef _DEBUG
	if (FAILED(Render_Components()))
		return E_FAIL;

	if (m_pGameInstance->GetKeyDown(eKeyCode::NUMPAD0))
		m_bRenderRTV = !m_bRenderRTV;
	
	if (m_bRenderRTV)
	{
		if (FAILED(Render_Debug()))
			return E_FAIL;
	}
	
#endif

	Render_UI();

	return S_OK;
}

void CRenderer::Clear()
{
	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pRenderObject : RenderObjects)
			Safe_Release(pRenderObject);
		RenderObjects.clear();
	}
}

void CRenderer::Draw_Objects(_uint iGroup)
{
	for (auto& pRenderObject : m_RenderObjects[iGroup])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[iGroup].clear();
}

void CRenderer::Bind_ViewProj()
{
	_float4x4 ViewMatrix = m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);

	for (auto& pShader : m_UsingShaders)
	{
		pShader->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
		pShader->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));
	}
}

HRESULT CRenderer::Copy_RenderTarget(const wstring& wstrTargetTag)
{
	//if (FAILED(m_pVIBuffer->Bind_Buffers()))
	//	return E_FAIL;


	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Copy")))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Bind_RT_SRV(wstrTargetTag, m_pPostProcessShader, "g_OriginTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return E_FAIL;

	Draw_Objects(RENDER_NONBLEND);

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	Draw_Objects(RENDER_NONLIGHT);

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	

	Draw_Objects(RENDER_BLEND);

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	if (nullptr == m_pVIBuffer_UI)
		m_pVIBuffer_UI = static_cast<CVIBuffer*>(m_pGameInstance->Clone_Component(0, L"Prototype_VIBuffer_Point"));

	if (FAILED(m_pVIBuffer_UI->Bind_Buffers()))
		return E_FAIL;

	Draw_Objects(RENDER_UI);

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	/* Shade */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Normal"), m_pDeferredShader, "g_NormalTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Depth"), m_pDeferredShader, "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Set_RawValue("g_ViewMatrixInv", &m_pGameInstance->Get_TransformFloat4x4_Inverse_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Set_RawValue("g_ProjMatrixInv", &m_pGameInstance->Get_TransformFloat4x4_Inverse_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Set_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Lights(m_pDeferredShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Deferred")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Diffuse"), m_pDeferredShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Shade"), m_pDeferredShader, "g_ShadeTexture")))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Depth"), m_pDeferredShader, "g_DepthTexture")))
	//	return E_FAIL;

	if (FAILED(m_pDeferredShader->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Effect()
{
	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(L"MRT_Effect")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Clear_Target(L"Target_Distortion")))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Clear_Target(L"Target_Glow")))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Clear_Target(L"Target_Effect_Bloom")))
	//	return E_FAIL;

	Draw_Objects(RENDER_EFFECT_NONBLEND);

	//m_RenderObjects[RENDERGROUP::RENDER_EFFECT_BLEND].sort([](CGameObject* pSrc, CGameObject* pDst)
	//	{
	//		return pSrc->Get_CamDistance() > pDst->Get_CamDistance();
	//	});

	Draw_Objects(RENDER_EFFECT_BLEND);


	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Glow()
{
	if (m_RenderObjects[RENDERGROUP::RENDER_GLOW].empty())
		return S_OK;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

#pragma region XBlur
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_BlurXGlow")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Glow"), m_pGlowShader, "g_PrevBlurXTexture")))
		return E_FAIL;

	if (FAILED(m_pGlowShader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
#pragma endregion

#pragma region YBlur
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_BlurYGlow")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_BlurXGlow"), m_pGlowShader, "g_PrevBlurYTexture")))
		return E_FAIL;

	if (FAILED(m_pGlowShader->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

#pragma endregion 

	if (FAILED(Copy_RenderTarget(L"Target_Deferred")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(L"MRT_Deferred")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Copy"), m_pPostProcessShader, "g_OriginTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_BlurYGlow"), m_pPostProcessShader, "g_GlowTextureAfterBlur")))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_GLOW])
		Safe_Release(pRenderObject);

	m_RenderObjects[RENDER_GLOW].clear();


	return S_OK;
}

HRESULT CRenderer::Render_Distortion()
{
	if (m_RenderObjects[RENDERGROUP::RENDER_DISTORTION].empty())
		return S_OK;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(Copy_RenderTarget(L"Target_Deferred")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(L"MRT_Deferred")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Copy"), m_pPostProcessShader, "g_OriginTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Distortion"), m_pPostProcessShader, "g_DistortionTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Begin(5)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))

		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_DISTORTION])
		Safe_Release(pRenderObject);

	m_RenderObjects[RENDER_DISTORTION].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Stencil()
{
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Stencil")))
		return E_FAIL;

	Draw_Objects(RENDER_STENCIL);

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_PostProcess()
{
	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	//if (FAILED(PostProcess_Bloom()))
	//	return E_FAIL;

	if (m_bRadialBlur)
	{
		if (FAILED(Render_RadialBlur()))
			return E_FAIL;
	}


	if (m_bColor_Inversion)
	{
		if (FAILED(Render_ColorInversion()))
			return E_FAIL;
	}
	
	if (FAILED(Render_Final()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::PostProcess_Bloom()
{
	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(TEXT("MRT_PostBloom"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Effect_Bloom"), m_pPostProcessShader, "g_PrevBloomTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_BloomUpSample" + to_wstring(m_iBloomLevel - 1), m_pPostProcessShader, "g_BloomTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_Deferred", m_pPostProcessShader, "g_OriginTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_RadialBlur()
{
	if (FAILED(Copy_RenderTarget(L"Target_Deferred")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Deferred")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Copy"), m_pPostProcessShader, "g_OriginTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Set_RawValue("g_fBlurStrength", &m_fNowBlurStrength, sizeof(_float))))
		return E_FAIL;
	
	if (FAILED(m_pPostProcessShader->Set_RawValue("g_fBlurRadius", &m_tRadialBlurDescs.fBlurRadius, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Set_RawValue("g_vBlurCenter", &Calc_BlurCenterUV(), sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_ColorInversion()
{
	if (FAILED(Copy_RenderTarget(L"Target_Deferred")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(L"MRT_Deferred")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Copy"), m_pPostProcessShader, "g_OriginTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Set_RawValue("g_fInversionRatio", &m_fInversionRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Begin(6)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Final()
{
	//if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_PostBloom"), m_pPostProcessShader, "g_FinalTexture")))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Deferred"), m_pPostProcessShader, "g_FinalTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Begin(4)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}


HRESULT CRenderer::Render_Components()
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	for (auto& pRenderComponent : m_RenderComponents)
	{
		if (nullptr != pRenderComponent)
			pRenderComponent->Render();

		Safe_Release(pRenderComponent);
	}

	m_RenderComponents.clear();

	return S_OK;
}



HRESULT CRenderer::Render_SampleForBloom()
{
	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(Render_BrightPass()))
		return E_FAIL;

	if (FAILED(Render_DownSample()))
		return E_FAIL;

	if (FAILED(Render_UpSample()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_BrightPass()
{
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_BrightPass")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_Deferred", m_pBloomShader, "g_PrevBrightPassTexture")))
		return E_FAIL;

	if (FAILED(m_pBloomShader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CRenderer::Render_DownSample()
{
	D3D11_VIEWPORT ViewPort = m_OriginViewPort;

	_int iDiv = 2;

	for (_uint i = 0; i < m_iBloomLevel; ++i)
	{
		iDiv = (_int)pow(2, i + 1);

		ViewPort.Width = 1280.f / iDiv;
		ViewPort.Height = 720.f / iDiv;

		m_pContext->RSSetViewports(1, &ViewPort);

		wstring MRTTag = L"MRT_BloomDownSample" + to_wstring(i);
		wstring BindRTTag = L"";
		if (0 == i)
			BindRTTag = L"Target_Effect_Bloom";
		else
			BindRTTag = L"Target_BloomDownSample" + to_wstring(i - 1);

		if (FAILED(m_pGameInstance->Begin_MRT(MRTTag)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Bind_RT_SRV(BindRTTag, m_pBloomShader, "g_ToSampleDownTexture")))
			return E_FAIL;

		if (FAILED(m_pBloomShader->Set_RawValue("g_iDiv", &iDiv, sizeof(_int))))
			return E_FAIL;

		if (FAILED(m_pBloomShader->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pVIBuffer->Render()))
			return E_FAIL;

		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_UpSample()
{
	D3D11_VIEWPORT ViewPort = m_OriginViewPort;

	_int iDiv = 4;

	for (_uint i = 0; i < m_iBloomLevel; ++i)
	{
		iDiv = (_int)pow(2, m_iBloomLevel - 1 - i);

		ViewPort.Width = 1280.f / iDiv;
		ViewPort.Height = 720.f / iDiv;
		m_pContext->RSSetViewports(1, &ViewPort);

		wstring MRTTag = L"MRT_BloomUpSample" + to_wstring(i);
		wstring BindRTTag = L"";

		if (0 == i)
			BindRTTag = L"Target_BloomDownSample" + to_wstring(m_iBloomLevel - 1);
		else
			BindRTTag = L"Target_BloomUpSample" + to_wstring(i - 1);

		if (FAILED(m_pGameInstance->Begin_MRT(MRTTag)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Bind_RT_SRV(BindRTTag, m_pBloomShader, "g_ToSampleUpTexture")))
			return E_FAIL;

		if (FAILED(m_pBloomShader->Set_RawValue("g_iDiv", &iDiv, sizeof(_int))))
			return E_FAIL;

		if (FAILED(m_pBloomShader->Begin(2)))
			return E_FAIL;

		if (FAILED(m_pVIBuffer->Render()))
			return E_FAIL;

		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;
	}

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Ready_Debug()
{
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Diffuse"), 300.f, 50.0f, 50.f, 50.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Normal"), 350.f, 50.0f, 50.f, 50.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Shade"), 400.f, 50.0f, 50.f, 50.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Deferred"), 300.f, 100.f, 50.f, 50.f)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_BrightPassed"), 350.f, 100.f, 50.f, 50.f)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_BloomDownSample0"), 400.f, 100.f, 50.f, 50.f)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_BloomDownSample1"), 450.f, 100.f, 50.f, 50.f)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_BloomDownSample2"), 500.f, 100.f, 50.f, 50.f)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_BloomUpSample0"), 550.f, 100.f, 50.f, 50.f)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_BloomUpSample1"), 600.f, 100.f, 50.f, 50.f)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_BloomUpSample2"), 650.f, 100.f, 50.f, 50.f)))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Glow"), 350.f, 100.f, 50.f, 50.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_BlurXGlow"), 400.f, 100.f, 50.f, 50.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_BlurYGlow"), 450.f, 100.f, 50.f, 50.f)))
		return E_FAIL;

	
	//
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_BloomDownSample2"), 500.f, 100.f, 50.f, 50.f)))


	return S_OK;
}

HRESULT CRenderer::Render_Debug()
{
	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_GameObjects"), m_pDeferredShader, m_pVIBuffer_Rect)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_LightAcc"), m_pDeferredShader, m_pVIBuffer_Rect)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_Deferred"), m_pDeferredShader, m_pVIBuffer_Rect)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_Glow"), m_pDeferredShader, m_pVIBuffer_Rect)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_BlurXGlow"), m_pDeferredShader, m_pVIBuffer_Rect)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_BlurYGlow"), m_pDeferredShader, m_pVIBuffer_Rect)))
		return E_FAIL;

	
	//if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_BrightPassed"), m_pDeferredShader, m_pVIBuffer_Rect)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_BloomDownSample0"), m_pDeferredShader, m_pVIBuffer_Rect)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_BloomDownSample1"), m_pDeferredShader, m_pVIBuffer_Rect)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_BloomDownSample2"), m_pDeferredShader, m_pVIBuffer_Rect)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_BloomUpSample0"), m_pDeferredShader, m_pVIBuffer_Rect)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_BloomUpSample1"), m_pDeferredShader, m_pVIBuffer_Rect)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_BloomUpSample2"), m_pDeferredShader, m_pVIBuffer_Rect)))
	//	return E_FAIL;
	//

	return S_OK;
}

#endif

HRESULT CRenderer::Ready_DefaultTargets(_uint iWidth, _uint iHeight)
{
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), iWidth, iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Copy"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Copy"), TEXT("Target_Copy"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_LightTargets(_uint iWidth, _uint iHeight)
{
	/* For.Target_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_DeferredTarget(_uint iWidth, _uint iHeight)
{
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Deferred"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Deferred"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_StencilTarget(_uint iWidth, _uint iHeight)
{
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Stencil"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Stencil"), TEXT("Target_Stencil"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_EffectTargets(_uint iWidth, _uint iHeight)
{
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Glow"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Distortion"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Bloom"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Deferred"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Glow"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Distortion"))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect_Bloom"))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Glow"))))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CRenderer::Ready_GlowTargets(_uint iWidth, _uint iHeight)
{
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurXGlow"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurYGlow"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_AfterGlow"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	//	return E_FAIL;

	
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurXGlow"), TEXT("Target_BlurXGlow"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurYGlow"), TEXT("Target_BlurYGlow"))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_AfterGlow"), TEXT("Target_AfterGlow"))))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Ready_BloomTargets(_uint iWidth, _uint iHeight)
{
	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_BrightPassed", iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_BrightPass", L"Target_BrightPassed")))
		return E_FAIL;


	for (_uint i = 0; i < m_iBloomLevel; ++i)
	{
		_uint iDiv = (_uint)pow(2, i + 1);

		wstring wstrTargetName = L"Target_BloomDownSample" + to_wstring(i);

		if (FAILED(m_pGameInstance->Add_RenderTarget(wstrTargetName, iWidth / iDiv, iHeight / iDiv, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
			return E_FAIL;
	}

	for (_uint i = 0; i < m_iBloomLevel; ++i)
	{
		_uint iDiv = (_uint)pow(2, m_iBloomLevel - 1 - i);

		wstring wstrTargetName = L"Target_BloomUpSample" + to_wstring(i);

		if (FAILED(m_pGameInstance->Add_RenderTarget(wstrTargetName, iWidth / iDiv, iHeight / iDiv, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
			return E_FAIL;
	}

	for (_uint i = 0; i < m_iBloomLevel; ++i)
	{
		wstring wstrDownMRTTag = L"MRT_BloomDownSample" + to_wstring(i);
		wstring wstrUpMRTTag = L"MRT_BloomUpSample" + to_wstring(i);

		wstring wstrDownRTTag = L"Target_BloomDownSample" + to_wstring(i);
		wstring wstrUpRTTag = L"Target_BloomUpSample" + to_wstring(i);

		if (FAILED(m_pGameInstance->Add_MRT(wstrDownMRTTag, wstrDownRTTag)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_MRT(wstrUpMRTTag, wstrUpRTTag)))
			return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_PostBloom", iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_PostBloom", L"Target_PostBloom")))
		return E_FAIL;

	return S_OK;
}


void CRenderer::Update_Color_Inversion(_float fTimeDelta)
{
	if (m_fInversionLerpTimeAcc > 0.f)
	{
		m_fInversionLerpTimeAcc -= fTimeDelta;

		_float fRatio = m_fInversionLerpTimeAcc / m_fInversionLerpTime;

		if (m_fInversionLerpTimeAcc < 0.f)
		{
			m_bColor_Inversion = false;
			m_fInversionLerpTimeAcc = 0.f;
			return;
		}
		m_fInversionRatio = XMVectorLerp(XMVectorZero(), XMVectorSet(1.f, 0.f, 0.f, 0.f), fRatio).m128_f32[0];
	}

}

HRESULT CRenderer::Ready_Buffers()
{
	m_pVIBuffer = CVIBuffer_Screen::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pVIBuffer_Rect = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer_Rect)
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Ready_Shaders()
{
	m_pDeferredShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements, false);
	if (nullptr == m_pDeferredShader)
		return E_FAIL;

	m_pBloomShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Bloom.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements, false);
	if (nullptr == m_pBloomShader)
		return E_FAIL;

	m_pGlowShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Glow.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements, false);
	if (nullptr == m_pGlowShader)
		return E_FAIL;

	m_pPostProcessShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_PostProcess.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements, false);
	if (nullptr == m_pPostProcessShader)
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Bind_Matrices()
{
	_float4x4 ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(1280.f, 720.f, 0.f, 1.f)));

	if (FAILED(m_pDeferredShader->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;


	return S_OK;
		
}

void CRenderer::Active_RadialBlur(const RADIALBLUR_DESCS& Descs)
{
	Safe_Release(m_tRadialBlurDescs.pActor);

	m_bRadialBlur = true;
	m_tRadialBlurDescs = Descs;
	m_fNowBlurStrength = Descs.fBlurStrength;
	m_fRadialBlurLerpTimeAcc = 0.f;

	Safe_AddRef(Descs.pActor);
}

void CRenderer::Inactive_RadialBlur(_uint iActorID, _float fLerpTime)
{
	if (false == m_bRadialBlur || m_fRadialBlurLerpTimeAcc > 0.f || m_tRadialBlurDescs.pActor->Get_ActorID() != iActorID)
		return;

	m_fRadialBlurLerpTime = m_fRadialBlurLerpTimeAcc = fLerpTime;
}


void CRenderer::Update_RadialBlur(_float fTimeDelta)
{
	if (nullptr != m_tRadialBlurDescs.pActor)
		XMStoreFloat3(&m_vBlurCenterWorld, m_tRadialBlurDescs.pActor->Get_Center());

	if (m_fRadialBlurLerpTimeAcc > 0.f)
	{
		m_fRadialBlurLerpTimeAcc -= fTimeDelta;

		_float fRatio = m_fRadialBlurLerpTimeAcc / m_fRadialBlurLerpTime;

		if (m_fRadialBlurLerpTimeAcc < 0.f)
		{
			m_bRadialBlur = false;
			m_fRadialBlurLerpTimeAcc = 0.f;
			return;
		}
		m_fNowBlurStrength = XMVectorLerp(XMVectorZero(), XMVectorSet(m_tRadialBlurDescs.fBlurStrength, 0.f, 0.f, 0.f), fRatio).m128_f32[0];
	}
}

_float2 CRenderer::Calc_BlurCenterUV()
{
	_matrix ViewMatrix = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	_matrix ProjMatrix = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);

	_vector vBlurCenter = XMLoadFloat3(&m_vBlurCenterWorld);
	vBlurCenter = XMVector3TransformCoord(vBlurCenter, ViewMatrix);
	vBlurCenter = XMVector3TransformCoord(vBlurCenter, ProjMatrix);

	vBlurCenter = vBlurCenter * 0.5f + XMVectorSet(0.5f, 0.5f, 0.f, 0.f);

	_float2 vCenterUV;
	XMStoreFloat2(&vCenterUV, vBlurCenter);

	return vCenterUV;
}

CRenderer * CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer*	pInstance = new CRenderer(pDevice, pContext);
	if (FAILED(pInstance->Initialize())) 
	{
		MSG_BOX(TEXT("Failed to Created : CRenderer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderer::Free()
{
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pDeferredShader);
	Safe_Release(m_pBloomShader);
	Safe_Release(m_pGlowShader);
	Safe_Release(m_pPostProcessShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pVIBuffer_Rect);

	Safe_Release(m_pVIBuffer_UI);

	for (auto& pShader : m_UsingShaders)
		Safe_Release(pShader);
	
	m_UsingShaders.clear();

	Safe_Release(m_tRadialBlurDescs.pActor);

	for (auto& RenderObjects : m_RenderObjects)
	{		
		for (auto& pRenderObject : RenderObjects)
			Safe_Release(pRenderObject);
		RenderObjects.clear();
	}

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
