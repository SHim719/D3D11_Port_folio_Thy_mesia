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
	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_UI"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	_matrix ViewportMatrix = XMMatrixSet(
		g_iWinSizeX * 0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -(g_iWinSizeY * 0.5f), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f, 0.f, 1.0f
	);

	XMStoreFloat4x4(&m_ViewportMatrix, ViewportMatrix);

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


_vector CUI::Convert_ScreenToRenderPos(_fvector vUIScreenPos)
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
	_vector vUIScreenPos = vUIWorldPos;

	_matrix ViewMatrix = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	_matrix ProjMatrix = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);

	vUIScreenPos = XMVector3TransformCoord(vUIScreenPos, ViewMatrix);
	vUIScreenPos = XMVector3TransformCoord(vUIScreenPos, ProjMatrix);
	vUIScreenPos = XMVector3TransformCoord(vUIScreenPos, XMLoadFloat4x4(&m_ViewportMatrix));

	return vUIScreenPos;
}

void CUI::Free()
{
	__super::Free();

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pUITexture);
}
