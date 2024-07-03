#include "UI_PlunderSlot.h"

#include "UI_SkillIcon.h"

#include "PlayerStats.h"

CUI_PlunderSlot::CUI_PlunderSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_PlunderSlot::CUI_PlunderSlot(const CUI_PlunderSlot& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_PlunderSlot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PlunderSlot::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_PlunderSlot"), TEXT("Texture"), (CComponent**)&m_pUITexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	m_pSkillIcon = static_cast<CUI_SkillIcon*>(m_pGameInstance->Clone_GameObject(L"Prototype_SkillIcon"));
	if (nullptr == m_pSkillIcon)
		return E_FAIL;

	CPlayerStats* pStats = (CPlayerStats*)pArg;
	pStats->Set_PlunderSlot(this);

	m_pTransform->Set_Scale({ 150.f, 150.f, 1.f });

	m_pTransform->Set_Position(Convert_ScreenToRenderPos(XMVectorSet(1200.f, 630.f, 1.f, 1.f)));

	return S_OK;
}


void CUI_PlunderSlot::Tick(_float fTimeDelta)
{
	
}

void CUI_PlunderSlot::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);

}

HRESULT CUI_PlunderSlot::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pUITexture->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pSkillIcon->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PlunderSlot::OnEnter_Layer(void* pArg)
{
	if (FAILED(__super::OnEnter_Layer(pArg)))
		return E_FAIL;

	m_bCreatedThisFrame = true;

	return S_OK;
}

void CUI_PlunderSlot::Update_SkillIcon(const SKILLTYPE eSkillType)
{
	m_pSkillIcon->Change_Texture(eSkillType);
}

CUI_PlunderSlot* CUI_PlunderSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_PlunderSlot* pInstance = new CUI_PlunderSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_PlunderSlot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_PlunderSlot::Clone(void* pArg)
{
	CUI_PlunderSlot* pInstance = new CUI_PlunderSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_PlunderSlot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PlunderSlot::Free()
{
	__super::Free();

	Safe_Release(m_pSkillIcon);
}
