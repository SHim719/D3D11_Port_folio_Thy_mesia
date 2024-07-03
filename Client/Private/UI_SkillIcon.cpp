#include "UI_SkillIcon.h"

CUI_SkillIcon::CUI_SkillIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_SkillIcon::CUI_SkillIcon(const CUI_SkillIcon& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_SkillIcon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SkillIcon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	m_pTransform->Set_Scale({ 150.f, 150.f, 1.f });

	m_pTransform->Set_Position(Convert_ScreenToRenderPos(XMVectorSet(1200.f, 630.f, 1.f, 1.f)));

	return S_OK;
}


void CUI_SkillIcon::Tick(_float fTimeDelta)
{
	
}

void CUI_SkillIcon::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);

}

HRESULT CUI_SkillIcon::Render()
{
	if (nullptr == m_pUITexture)
		return S_OK;

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

HRESULT CUI_SkillIcon::OnEnter_Layer(void* pArg)
{
	if (FAILED(__super::OnEnter_Layer(pArg)))
		return E_FAIL;

	m_bCreatedThisFrame = true;

	return S_OK;
}

void CUI_SkillIcon::Change_Texture(const SKILLTYPE eSkillType)
{
	wstring wstrTextureTag = L"";
	Safe_Release(m_pUITexture);

	switch (eSkillType)
	{
	case AXE:
		wstrTextureTag = L"Prototype_Texture_SkillIcon_Axe";
		break;
	case HAMMER:
		wstrTextureTag = L"Prototype_Texture_SkillIcon_Hammer";
		break;
	}
	
	if (wstrTextureTag.size())
		m_pUITexture = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, wstrTextureTag));
}

CUI_SkillIcon* CUI_SkillIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_SkillIcon* pInstance = new CUI_SkillIcon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_SkillIcon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_SkillIcon::Clone(void* pArg)
{
	CUI_SkillIcon* pInstance = new CUI_SkillIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_SkillIcon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_SkillIcon::Free()
{
	__super::Free();

}
