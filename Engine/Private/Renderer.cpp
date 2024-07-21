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

	/* For.Target_Diffuse */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;

	if (FAILED(Ready_BloomTargets((_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height)))
		return E_FAIL;


	m_pVIBuffer = CVIBuffer_Screen::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pVIBuffer_Rect = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer_Rect)
		return E_FAIL;

	m_pDifferedShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements, false);
	if (nullptr == m_pDifferedShader)
		return E_FAIL;

	m_pPostProcessShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_PostProcessing.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements, false);
	if (nullptr == m_pPostProcessShader)
		return E_FAIL;

	//_matrix WorldMatrix = XMMatrixIdentity();
	//WorldMatrix.r[0].m128_f32[0] = ViewportDesc.Width;
	//WorldMatrix.r[1].m128_f32[1] = ViewportDesc.Height;
	//
	//XMStoreFloat4x4(&m_WorldMatrix, XMMatrixTranspose(WorldMatrix));
	//
	_float4x4 ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f)));
	
	if (FAILED(m_pDifferedShader->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	
	if (FAILED(m_pDifferedShader->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	//
	//if (FAILED(m_pPostProcessShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
	//	return E_FAIL;
	//
	//if (FAILED(m_pPostProcessShader->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
	//	return E_FAIL;
	//
	//if (FAILED(m_pPostProcessShader->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
	//	return E_FAIL;


#ifdef _DEBUG
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Diffuse"), 500.f, 50.0f, 50.f, 50.f)))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Normal"), 550.f, 50.0f, 50.f, 50.f)))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Shade"), 600.f, 50.0f, 50.f, 50.f)))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_ReadyBloom"), 500.f, 50.0f, 50.f,50.f)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_BrightPassed"), 600.f, 50.0f, 50.f, 50.f)))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_BlurX"), 700.f, 100.0f, 200.f, 200.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_BlurY"), 900.f, 100.0f, 200.f, 200.f)))
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

HRESULT CRenderer::Draw()
{
	m_pGameInstance->Update_ViewProj();
	Bind_ViewProj();

	if (FAILED(Render_NonBlend()))
		return E_FAIL;

	/* 빛연산한 결과를 Shade타겟에 저장받는다. */
/* Shade타겟을 장치에 바인딩작업이 필요하다.*/
/* 뭔가를 그리낟 == 사각형 버퍼를 직교투영으로 그린다. shade타겟을 꽉채워서 그리는 사가형을 그리겠다. */
/* 사각형을 그릴때 빛 정보와 디퓨즈와 노말을 이용해서 빛연산한 결과를 Shade타겟에 그려줄꺼다. */
	if (FAILED(Render_LightAcc()))
		return E_FAIL;

	/* Diffuse Shade 두개의 타겟을 곱해서 백버퍼에 그려준다. */
	/* 사각형을 직교투영으로 그린다. 그리위해서는 쉐이더가 필요해., */
	/* 이 쉐이더에 Diffuse SRV, Shade SRV두개를 던져서 곱해서 찍는다. */
	if (FAILED(Render_Deferred()))
		return E_FAIL;

	Render_NonLight();
	Render_Blend();

	Render_Effect();
	
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
	//Alpha Sorting

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

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Normal"), m_pDifferedShader, "g_NormalTexture")))
		return E_FAIL;

	/* 셋팅해놓은 쉐이트 타겟에 모드,ㄴ 픽셀을 채워주기위해서. */
	/* 쉐이드 타겟의 사이즈만큼 직교투ㅠ영된 사가셯ㅇ 버퍼를 그린다. */
	if (FAILED(m_pDifferedShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	/* 빛 갯수만큼 사각형을 그리낟. */
	if (FAILED(m_pGameInstance->Render_Lights(m_pDifferedShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Diffuse"), m_pDifferedShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Shade"), m_pDifferedShader, "g_ShadeTexture")))
		return E_FAIL;

	if (FAILED(m_pDifferedShader->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	return m_pVIBuffer->Render();
}

HRESULT CRenderer::Render_Effect()
{
	if (0 == m_RenderObjects[RENDER_EFFECT].size())
		return E_FAIL;
	//
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom"))))
		return E_FAIL;

	Draw_Objects(RENDER_EFFECT);

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	Render_EffectBloom();

	return S_OK;
}

HRESULT CRenderer::Render_EffectBloom()
{
	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	Render_BrightPass();
	Render_Blur();
	Render_Bloom();

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

HRESULT CRenderer::Render_BrightPass()
{
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_BrightPass")))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_ReadyBloom"), m_pPostProcessShader, "g_ReadyBrightPassTexture")))
		return E_FAIL;
	
	if (FAILED(m_pPostProcessShader->Begin(0)))
		return E_FAIL;
	
	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Blur()
{
	m_pContext->RSSetViewports(1, &m_DownScalingViewPort);

	for (_int i = 0; i < 10; ++i)
	{
		if (i == 0)
		{
			if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_BlurX")))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_BrightPassed"), m_pPostProcessShader, "g_BlurXTexture")))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pGameInstance->Begin_MRT_NoClear(L"MRT_BlurX")))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_BlurX"), m_pPostProcessShader, "g_BlurXTexture")))
				return E_FAIL;
		}

		if (FAILED(m_pPostProcessShader->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pVIBuffer->Render()))
			return E_FAIL;

		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;

		// BlurY

		if (i == 0)
		{
			if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_BlurY")))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pGameInstance->Begin_MRT_NoClear(L"MRT_BlurY")))
				return E_FAIL;
		}
		
		if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_BlurY"), m_pPostProcessShader, "g_BlurYTexture")))
			return E_FAIL;

		if (FAILED(m_pPostProcessShader->Begin(2)))
			return E_FAIL;

		if (FAILED(m_pVIBuffer->Render()))
			return E_FAIL;

		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;
	}
	
	m_pContext->RSSetViewports(1, &m_OriginViewPort);

	return S_OK;
}

HRESULT CRenderer::Render_Bloom()
{
	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_BlurX"), m_pPostProcessShader, "g_BlurXTexture")))
		return E_FAIL; 

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_BrightPassed"), m_pPostProcessShader, "g_BrightPassedTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_GameObjects"), m_pDifferedShader, m_pVIBuffer_Rect)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_LightAcc"), m_pDifferedShader, m_pVIBuffer_Rect)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_ReadyBloom"), m_pDifferedShader, m_pVIBuffer_Rect)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_BrightPassed"), m_pDifferedShader, m_pVIBuffer_Rect)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_BlurX"), m_pDifferedShader, m_pVIBuffer_Rect)))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_BlurY"), m_pDifferedShader, m_pVIBuffer_Rect)))
		return E_FAIL;

	return S_OK;
}
#endif

HRESULT CRenderer::Ready_BloomTargets(_uint iWidth, _uint iHeight)
{
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_ReadyBloom"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BrightPassed"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurX"), iWidth / 16, iHeight / 16, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurY"), iWidth / 16, iHeight / 16, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BrightPass"), TEXT("Target_BrightPassed"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurX"), TEXT("Target_BlurY"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurY"), TEXT("Target_BlurX"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom"), TEXT("Target_ReadyBloom"))))
		return E_FAIL;

	ZeroMemory(&m_DownScalingViewPort, sizeof(D3D11_VIEWPORT));
	m_DownScalingViewPort.TopLeftX = 0;
	m_DownScalingViewPort.TopLeftY = 0;
	m_DownScalingViewPort.Width = _float(iWidth) / 16.f;
	m_DownScalingViewPort.Height = _float(iHeight) / 16.f;
	m_DownScalingViewPort.MinDepth = 0.0f;
	m_DownScalingViewPort.MaxDepth = 1.0f;

	return S_OK;
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

	Safe_Release(m_pDifferedShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pVIBuffer_Rect);

	Safe_Release(m_pVIBuffer_UI);

	for (auto& pShader : m_UsingShaders)
		Safe_Release(pShader);
	
	m_UsingShaders.clear();

	for (auto& RenderObjects : m_RenderObjects)
	{		
		for (auto& pRenderObject : RenderObjects)
			Safe_Release(pRenderObject);
		RenderObjects.clear();
	}

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
