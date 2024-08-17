#include "UI_PW.h"

#include "UI_Manager.h"

#include "Player.h"
#include "PlayerStats.h"

#include "FadeScreen.h"

CUI_PW::CUI_PW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_PW::CUI_PW(const CUI_PW& rhs)
	: CUI(rhs)
{
}


HRESULT CUI_PW::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	Init_UIMatrices();
	Init_UIPos();

	m_pPlayerStats = GET_PLAYER->Get_PlayerStats();
	Safe_AddRef(m_pPlayerStats);

	m_bSkillActived[CLAW_SHORT] = true;
	m_bSkillActived[CLAW_LONG] = true;

	return S_OK;
}


void CUI_PW::Tick(_float fTimeDelta)
{
	if (KEY_DOWN(eKeyCode::Left))
	{
		m_eNowSelected = UI_PlagueWeapons((m_eNowSelected + WEAPON_END - 1) % WEAPON_END);
	}
	else if (KEY_DOWN(eKeyCode::Right))
	{
		m_eNowSelected = UI_PlagueWeapons((m_eNowSelected + 1) % WEAPON_END);
	}
	else if (KEY_DOWN(eKeyCode::Up))
	{
		m_eNowSelected = UI_PlagueWeapons((m_eNowSelected + WEAPON_END - 3) % WEAPON_END);
	}
	else if (KEY_DOWN(eKeyCode::Down))
	{
		m_eNowSelected = UI_PlagueWeapons((m_eNowSelected + 3) % WEAPON_END);
	}
	else if (KEY_DOWN(eKeyCode::Enter))
	{
		Change_UsingSkill();
	}
	else if (KEY_DOWN(eKeyCode::ESC))
	{
		Exit();
	}

}

void CUI_PW::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
}

HRESULT CUI_PW::Render()
{
	if (FAILED(Render_BG()))
		return E_FAIL;

	if (FAILED(Render_InactiveCover()))
		return E_FAIL;

	if (FAILED(Render_Selected()))
		return E_FAIL;

	if (FAILED(Render_UsingSkillIcon()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PW::OnEnter_Layer(void* pArg)
{
	UIMGR->Inactive_UI("UI_Menu");

	m_bReturnToPool = false;

	m_eNowSelected = AXE;

	Update_SkillActived();

	return S_OK;
}

HRESULT CUI_PW::Render_BG()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_BGWorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pUITexture->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(0)))
		return E_FAIL;

	return m_pVIBuffer->Render();
}

HRESULT CUI_PW::Render_InactiveCover()
{
	m_pTransform->Set_Scale(m_vIconScale);
	m_fAlpha = 0.4f;

	if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	for (_uint i = 0; i < UI_PlagueWeapons::WEAPON_END; ++i)
	{
		if (false == m_bSkillActived[i])
		{
			m_pTransform->Set_Position(XMLoadFloat4(&m_vIconPositions[i]));
			
			if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
				return E_FAIL;

			if (FAILED(m_pInactiveCoverTexture->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
				return E_FAIL;

			if (FAILED(m_pShader->Begin(3)))
				return E_FAIL;

			if (FAILED(m_pVIBuffer->Render()))
				return E_FAIL;
		}
	}


	return S_OK;
}

HRESULT CUI_PW::Render_Selected()
{
	m_pTransform->Set_Scale(m_vSelectedScale);
	m_fAlpha = 1.f;
	m_pTransform->Set_Position(XMLoadFloat4(&m_vIconPositions[m_eNowSelected]));

	if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pSelectedTexture->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(3)))
		return E_FAIL;

	return m_pVIBuffer->Render();
}

HRESULT CUI_PW::Render_UsingSkillIcon()
{
	m_pTransform->Set_Scale(m_vUsingIconScale);
	m_fAlpha = 1.f;

	if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	for (_uint i = 0; i < 2; ++i)
	{
		if (nullptr == m_pUsingSkillTextures[i])
			continue;

		m_pTransform->Set_Position(XMLoadFloat4(&m_vUsingIconPositions[i]));

		if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pUsingSkillTextures[i]->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(3)))
			return E_FAIL;

		if (FAILED(m_pVIBuffer->Render()))
			return E_FAIL;
	}

	return S_OK;
}

void CUI_PW::Exit()
{
	CFadeScreen::FADEDESC FadeDesc{};
	FadeDesc.eFadeColor = CFadeScreen::BLACK;
	FadeDesc.fFadeOutSpeed = 3.f;
	FadeDesc.fFadeInSpeed = 10.f;
	FadeDesc.pCallback_FadeOutEnd = bind(&CUI_Manager::Active_UI, CUI_Manager::Get_Instance(), "UI_Menu", nullptr);
	FadeDesc.pCallback_FadeInStart = bind(&CGameObject::Set_ReturnToPool, this, true);

	UIMGR->Active_UI("FadeScreen", &FadeDesc);
}

wstring CUI_PW::Get_SkillIconTag()
{
	wstring wstrTag = L"";

	switch (m_eNowSelected)
	{
	case UI_PlagueWeapons::AXE:
		wstrTag = L"Prototype_Texture_SkillIcon_Axe";
		break;
	case UI_PlagueWeapons::HAMMER:
		wstrTag = L"Prototype_Texture_SkillIcon_Hammer";
		break;
	case UI_PlagueWeapons::SPEAR:
		wstrTag = L"Prototype_Texture_SkillIcon_Spear";
		break;
	case UI_PlagueWeapons::TWIN:
		wstrTag = L"Prototype_Texture_SkillIcon_TwinBlade";
		break;
	case UI_PlagueWeapons::CLAW_LONG:
		wstrTag = L"Prototype_Texture_SkillIcon_Claw_Long";
		break;
	case UI_PlagueWeapons::CLAW_SHORT:
		wstrTag = L"Prototype_Texture_SkillIcon_Claw_Short";
		break;
	}

	return wstrTag;
}

SKILLTYPE CUI_PW::Get_SelectedToSkillType()
{
	SKILLTYPE eType;

	switch (m_eNowSelected)
	{
	case UI_PlagueWeapons::AXE:
		eType = SKILLTYPE::AXE;
		break;
	case UI_PlagueWeapons::HAMMER:
		eType = SKILLTYPE::HAMMER;
		break;
	case UI_PlagueWeapons::SPEAR:
		eType = SKILLTYPE::SPEAR;
		break;
	case UI_PlagueWeapons::TWIN:
		eType = SKILLTYPE::TWINBLADE;
		break;
	}

	return eType;
}

HRESULT CUI_PW::Ready_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Window_PW"), TEXT("PW_Texture"), (CComponent**)&m_pUITexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Slot_Inactive"), TEXT("Cover_Texture"), (CComponent**)&m_pInactiveCoverTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Slot_Selected"), TEXT("Selected_Texture"), (CComponent**)&m_pSelectedTexture)))
		return E_FAIL;
	
	m_pUsingSkillTextures[1] = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, L"Prototype_Texture_SkillIcon_Claw_Long"));

	return S_OK;
}


void CUI_PW::Init_UIMatrices()
{
	_matrix WorldMatrix = XMMatrixIdentity();
	_float3 vScale = { 1280.f, 720.f, 1.f };

	WorldMatrix.r[0] *= vScale.x;
	WorldMatrix.r[1] *= vScale.y;

	XMStoreFloat4x4(&m_BGWorldMatrix, XMMatrixTranspose(WorldMatrix));
}

void CUI_PW::Init_UIPos()
{
	XMStoreFloat4(&m_vIconPositions[AXE], Convert_ScreenToRenderPos(XMVectorSet(317.f, 178.f, 1.f, 1.f)));
	XMStoreFloat4(&m_vIconPositions[HAMMER], Convert_ScreenToRenderPos(XMVectorSet(460.f, 178.f, 1.f, 1.f)));
	XMStoreFloat4(&m_vIconPositions[CLAW_LONG], Convert_ScreenToRenderPos(XMVectorSet(783.f, 188.f, 1.f, 1.f)));
	XMStoreFloat4(&m_vIconPositions[SPEAR], Convert_ScreenToRenderPos(XMVectorSet(317.f, 385.f, 1.f, 1.f))); 
	XMStoreFloat4(&m_vIconPositions[TWIN], Convert_ScreenToRenderPos(XMVectorSet(460.f, 385.f, 1.f, 1.f))); 
	XMStoreFloat4(&m_vIconPositions[CLAW_SHORT], Convert_ScreenToRenderPos(XMVectorSet(783.f, 356.f, 1.f, 1.f)));

	XMStoreFloat4(&m_vUsingIconPositions[0], Convert_ScreenToRenderPos(XMVectorSet(386.f, 576.f, 1.f, 1.f)));
	XMStoreFloat4(&m_vUsingIconPositions[1], Convert_ScreenToRenderPos(XMVectorSet(785.f, 570.f, 1.f, 1.f)));
	
}

void CUI_PW::Update_SkillActived()
{
	m_bSkillActived[AXE] = m_pPlayerStats->Is_SkillActived(SKILLTYPE::AXE);
	m_bSkillActived[HAMMER] = m_pPlayerStats->Is_SkillActived(SKILLTYPE::HAMMER);
	m_bSkillActived[SPEAR] = m_pPlayerStats->Is_SkillActived(SKILLTYPE::SPEAR);
	m_bSkillActived[TWIN] = m_pPlayerStats->Is_SkillActived(SKILLTYPE::TWINBLADE);
}

void CUI_PW::Change_UsingSkill()
{
	if (m_eNowSelected == CLAW_SHORT || m_eNowSelected == CLAW_LONG)
	{
		Safe_Release(m_pUsingSkillTextures[1]);

		m_pUsingSkillTextures[1] = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, Get_SkillIconTag()));

		m_pPlayerStats->Set_UseShortClaw(m_eNowSelected == CLAW_SHORT);
	}
	else if (m_pPlayerStats->Is_SkillActived(Get_SelectedToSkillType()))
	{
		Safe_Release(m_pUsingSkillTextures[0]);

		m_pUsingSkillTextures[0] = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, Get_SkillIconTag()));

		m_pPlayerStats->Update_UsingSkill(Get_SelectedToSkillType());
	}
}


CUI_PW* CUI_PW::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_PW* pInstance = new CUI_PW(pDevice, pContext);

	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_PW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_PW::Clone(void* pArg)
{
	CUI_PW* pInstance = new CUI_PW(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_PW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PW::Free()
{
	__super::Free();

	Safe_Release(m_pPlayerStats);

	Safe_Release(m_pUsingSkillTextures[0]);
	Safe_Release(m_pUsingSkillTextures[1]);
	Safe_Release(m_pInactiveCoverTexture);
	Safe_Release(m_pSelectedTexture);
}
