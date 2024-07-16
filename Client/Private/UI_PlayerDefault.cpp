#include "UI_PlayerDefault.h"
#include "UI_PlayerBar.h"
#include "UI_SoulBar.h"
#include "UI_SkillSlot.h"
#include "UI_PlunderSlot.h"

#include "Player.h"
#include "PlayerStats.h"

CUI_PlayerDefault::CUI_PlayerDefault(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_PlayerDefault::CUI_PlayerDefault(const CUI_PlayerDefault& rhs)
	: CUI(rhs)
{
}
HRESULT CUI_PlayerDefault::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	m_pPlayerBar	= CUI_PlayerBar::Create(m_pDevice, m_pContext);
	m_pSoulBar		= CUI_SoulBar::Create(m_pDevice, m_pContext);
	m_pSkillSlot	= CUI_SkillSlot::Create(m_pDevice, m_pContext);
	m_pPlunderSlot	= CUI_PlunderSlot::Create(m_pDevice, m_pContext);

	GET_PLAYER->Get_PlayerStats()->Set_PlayerDefaultUI(this);

	return S_OK;
}

void CUI_PlayerDefault::Tick(_float fTimeDelta)
{
	m_pSoulBar->Tick(fTimeDelta);
}

void CUI_PlayerDefault::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
}

HRESULT CUI_PlayerDefault::Render()
{
	if (FAILED(m_pPlayerBar->Render()))
		return E_FAIL;

	if (FAILED(m_pSoulBar->Render()))
		return E_FAIL;

	//if (FAILED(m_pSkillSlot->Render()))
	//	return E_FAIL;

	if (FAILED(m_pPlunderSlot->Render()))
		return E_FAIL;

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pPlayerBar->Draw_HpText();
	m_pSoulBar->Draw_SoulText();

	return S_OK;
}


void CUI_PlayerDefault::Update_HpBar(_int iMaxHp, _int iHp)
{
	m_pPlayerBar->Update_PlayerHp(iMaxHp, iHp);
}

void CUI_PlayerDefault::Update_SoulBar(_int iSoulCount)
{
	m_pSoulBar->Set_SoulNumber(iSoulCount);
}

void CUI_PlayerDefault::Update_SkillSlot()
{
}

void CUI_PlayerDefault::Update_PlunderSlot(SKILLTYPE eSkilType)
{
	m_pPlunderSlot->Update_SkillIcon(eSkilType);
}


CUI_PlayerDefault* CUI_PlayerDefault::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_PlayerDefault* pInstance = new CUI_PlayerDefault(pDevice, pContext);

	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_PlayerDefault"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_PlayerDefault::Clone(void* pArg)
{
	return nullptr;
}


void CUI_PlayerDefault::Free()
{
	__super::Free();

	Safe_Release(m_pPlayerBar);
	Safe_Release(m_pSoulBar);
	Safe_Release(m_pSkillSlot);
	Safe_Release(m_pPlunderSlot);
}
