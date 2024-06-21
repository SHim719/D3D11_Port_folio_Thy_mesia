#include "UI.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI::CUI(const CUI& rhs)
	: CGameObject(rhs)
{
}


HRESULT CUI::Initialize_Prototype()
{
	_float4x4 ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f)));

	m_pShader->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pShader->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	_matrix ViewportMatrix = XMMatrixSet(
		g_iWinSizeX * 0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -(g_iWinSizeY * 0.5f), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f, 0.f, 1.0f
	);

	XMStoreFloat4x4(&m_ViewportMatrix, ViewportMatrix);

	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_UI"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	return S_OK;
}

void CUI::Tick(_float fTimeDelta)
{
}

void CUI::LateTick(_float fTimeDelta)
{
}

HRESULT CUI::Render()
{
	return S_OK;
}

void CUI::On_UIActive(void* pArg)
{
}

_vector CUI::Convert_ScreenToWorld(_fvector vUIScreenPos)
{
	/*
	m_fX = SX - WINCX* 0.5f;
	m_fY = WINCY * 0.5f - SY;
	*/
	
	return XMVectorSet(vUIScreenPos.m128_f32[0] - _float(g_iWinSizeX) * 0.5f
		, _float(g_iWinSizeY) * 0.5f - vUIScreenPos.m128_f32[1]
		, 1.f, 1.f);
}

_vector CUI::Convert_WorldToScreen(_fvector vUIWorldPos)
{
	/*
	SX = m_fX + WINCX * 0.5f,
	SY = WINCY * 0.5f - m_fY;
	*/

	return XMVectorSet(vUIWorldPos.m128_f32[0] + _float(g_iWinSizeX) * 0.5f
		, _float(g_iWinSizeY) * 0.5f - vUIWorldPos.m128_f32[1]
		, XMVectorGetZ(vUIWorldPos), 1.f);
}

void CUI::Free()
{
	__super::Free();

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pUITexture);
}
