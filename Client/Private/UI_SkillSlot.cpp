#include "UI_SkillSlot.h"


CUI_DefaultSkillSlot::CUI_DefaultSkillSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_DefaultSkillSlot::CUI_DefaultSkillSlot(const CUI_DefaultSkillSlot& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_DefaultSkillSlot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_DefaultSkillSlot::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_LockOn"), TEXT("Texture"), (CComponent**)&m_pUITexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	m_pTransform->Set_Scale({ 100.f, 100.f, 1.f });

	return S_OK;
}


void CUI_DefaultSkillSlot::Tick(_float fTimeDelta)
{

}

void CUI_DefaultSkillSlot::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);

}

HRESULT CUI_DefaultSkillSlot::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pUITexture->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_DefaultSkillSlot::OnEnter_Layer(void* pArg)
{
	if (FAILED(__super::OnEnter_Layer(pArg)))
		return E_FAIL;

	return S_OK;
}

CUI_DefaultSkillSlot* CUI_DefaultSkillSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_DefaultSkillSlot* pInstance = new CUI_DefaultSkillSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_DefaultSkillSlot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_DefaultSkillSlot::Clone(void* pArg)
{
	CUI_DefaultSkillSlot* pInstance = new CUI_DefaultSkillSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_DefaultSkillSlot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_DefaultSkillSlot::Free()
{
	__super::Free();

}
