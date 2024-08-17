#include "UI_PotionSlot.h"

#include "PlayerStats.h"

CUI_PotionSlot::CUI_PotionSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_PotionSlot::CUI_PotionSlot(const CUI_PotionSlot& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_PotionSlot::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransform->Set_Scale({ 110.f, 40.f, 1.f });
	m_pTransform->Set_Position(Convert_ScreenToRenderPos(XMVectorSet(1000.f, 628.f, 1.f, 1.f)));


	m_vPotionTextPos = { 990.f,  612.f };

	return S_OK;
}


HRESULT CUI_PotionSlot::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pUITexture->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return m_pVIBuffer->Bind_Buffers();
}

HRESULT CUI_PotionSlot::Render_PotionCount()
{
	m_pGameInstance->Render_Font(L"Main_Font15", m_wstrPotionText, m_vPotionTextPos);

	return m_pVIBuffer->Bind_Buffers();
}


HRESULT CUI_PotionSlot::Ready_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_PotionSlot"), TEXT("Texture"), (CComponent**)&m_pUITexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;


	return S_OK;
}

CUI_PotionSlot* CUI_PotionSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_PotionSlot* pInstance = new CUI_PotionSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_PotionSlot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_PotionSlot::Clone(void* pArg)
{
	CUI_PotionSlot* pInstance = new CUI_PotionSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_PotionSlot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PotionSlot::Free()
{
	__super::Free();

}
