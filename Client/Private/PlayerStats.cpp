#include "PlayerStats.h"
#include "UI_Manager.h"
#include "UI_PlayerDefault.h"

CPlayerStats::CPlayerStats()
{
}


void CPlayerStats::Update_PlunderSkill(const SKILLTYPE ePlunderSkill)
{
	m_ePlunderSkill = ePlunderSkill;
	m_pPlayerUI->Update_PlunderSlot(ePlunderSkill);
}

_int CPlayerStats::Increase_Hp(_int iHp)
{
	m_iHp += iHp;	
	m_iHp = clamp(m_iHp, 0, m_iMaxHp);

	m_pPlayerUI->Update_HpBar(m_iMaxHp, m_iHp);
	return m_iHp;
}

void CPlayerStats::Increase_MaxHp(_int iHp)
{
	m_iMaxHp += iHp;

	m_pPlayerUI->Update_HpBar(m_iMaxHp, m_iHp);
}

void CPlayerStats::SetHp_Full()
{
	m_iHp = m_iMaxHp;
	m_pPlayerUI->Update_HpBar(m_iMaxHp, m_iHp);
}

void CPlayerStats::Increase_SoulCount(_int iSoul)
{
	m_iSoulCount += iSoul;
	if (iSoul > 0)
		m_pPlayerUI->Update_SoulBar(m_iSoulCount);
}

ATTACKDESC CPlayerStats::Get_NormalAttackDesc() const
{
	ATTACKDESC AttackDesc;
	AttackDesc.iDamage = (m_iStrength * 2);
	AttackDesc.iPlagueDamage = m_iStrength >> 2;

	return AttackDesc;
}

ATTACKDESC CPlayerStats::Get_PlagueAttackDesc() const
{
	ATTACKDESC AttackDesc;
	AttackDesc.iDamage = m_iStrength >> 2;
	AttackDesc.iPlagueDamage = m_bShortClaw == true ? m_iPlague : m_iPlague * 2;

	return AttackDesc;
}

void CPlayerStats::Set_PlayerDefaultUI(CUI_PlayerDefault* pUI)
{
	m_pPlayerUI = pUI;
	Safe_AddRef(m_pPlayerUI);

	m_pPlayerUI->Update_HpBar(m_iMaxHp, m_iHp);
	//m_pPlayerUI->Update_SoulBar(m_iSoulCount);
}

CPlayerStats* CPlayerStats::Create()
{
	return new CPlayerStats;
}

void CPlayerStats::Free()
{
	Safe_Release(m_pPlayerUI);
}
