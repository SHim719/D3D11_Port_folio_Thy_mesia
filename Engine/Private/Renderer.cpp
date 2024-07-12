#include "..\Public\Renderer.h"
#include "GameObject.h"

#include "GameInstance.h"

#include "VIBuffer_Rect.h"
#include "Shader.h"


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

	/* For.Target_Diffuse */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;


	/* For.MRT_GameObjects ( Diffuse + Normal ) */
	/* �� �׷쿡 �߰��� ����Ÿ�ٵ��� ��ü�κ��� �޾ƿ;��� ������ �Ǹ��Ѥ�.�� */
	/* ��ü���� �׸��� ������ ���ε� �Ǿ���� Ÿ�ٵ��̴�. */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;

	/* For.MRT_LightAcc ( Shade )*/
	/* �� �׷쿡 �߰��� ����Ÿ�ٵ��� ����κ��� �޾ƿ;��� ������ �Ǹ��Ѥ�.�� */
	/* �������� �ϱ� ������ ���ε� �Ǿ���� Ÿ�ٵ��̴�. */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;


	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements, false);
	if (nullptr == m_pShader)
		return E_FAIL;

	_matrix WorldMatrix = XMMatrixIdentity();
	WorldMatrix.r[0].m128_f32[0] = ViewportDesc.Width;
	WorldMatrix.r[1].m128_f32[1] = ViewportDesc.Height;
	
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixTranspose(WorldMatrix));

	_float4x4 ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f)));

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Diffuse"), 500.f, 50.0f, 50.f, 50.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Normal"), 550.f, 50.0f, 50.f, 50.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Shade"), 600.f, 50.0f, 50.f, 50.f)))
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

	/* �������� ����� ShadeŸ�ٿ� ����޴´�. */
/* ShadeŸ���� ��ġ�� ���ε��۾��� �ʿ��ϴ�.*/
/* ������ �׸��� == �簢�� ���۸� ������������ �׸���. shadeŸ���� ��ä���� �׸��� �簡���� �׸��ڴ�. */
/* �簢���� �׸��� �� ������ ��ǻ��� �븻�� �̿��ؼ� �������� ����� ShadeŸ�ٿ� �׷��ٲ���. */
	if (FAILED(Render_LightAcc()))
		return E_FAIL;

	/* Diffuse Shade �ΰ��� Ÿ���� ���ؼ� ����ۿ� �׷��ش�. */
	/* �簢���� ������������ �׸���. �׸����ؼ��� ���̴��� �ʿ���., */
	/* �� ���̴��� Diffuse SRV, Shade SRV�ΰ��� ������ ���ؼ� ��´�. */
	if (FAILED(Render_Deferred()))
		return E_FAIL;

	Render_NonLight();
	Render_Blend();
	Render_UI();

#ifdef _DEBUG
	if (FAILED(Render_Components()))
		return E_FAIL;

	if (FAILED(Render_Debug()))
		return E_FAIL;

#endif

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

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return E_FAIL;

	/* �����س��� ����Ʈ Ÿ�ٿ� ���,�� �ȼ��� ä���ֱ����ؼ�. */
	/* ���̵� Ÿ���� �����ŭ �������п��� �簡�q�� ���۸� �׸���. */
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	/* �� ������ŭ �簢���� �׸���. */
	if (FAILED(m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	return m_pVIBuffer->Render();
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

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}
#endif

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

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);

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
