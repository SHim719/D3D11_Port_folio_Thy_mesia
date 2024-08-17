#include "PlayerStats.h"
#include "UI_Manager.h"
#include "UI_PlayerDefault.h"


CPlayerStats::CPlayerStats()
{
}


void CPlayerStats::Active_Skill(SKILLTYPE eSkill)
{
	if (true == m_bSkillActived[eSkill])
		return;

	m_bSkillActived[eSkill] = true;
}

void CPlayerStats::Add_PotionCount(_int iCount)
{
	m_iNowPotionCount = clamp(m_iNowPotionCount + iCount, 0, m_iMaxPotionCount);
	m_pPlayerUI->Update_PotionSlot(m_iNowPotionCount);
}

void CPlayerStats::Update_PlunderSkill(const SKILLTYPE ePlunderSkill)
{
	m_ePlunderSkill = ePlunderSkill;
	m_pPlayerUI->Update_PlunderSlot(ePlunderSkill);
}

void CPlayerStats::Update_UsingSkill(const SKILLTYPE eSkill)
{
	m_eUsingSkill = eSkill;
	m_pPlayerUI->Update_SkillSlot(eSkill);
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
	AttackDesc.ePlayerAttackType = ATTACK_NORMAL;
	AttackDesc.iDamage = m_iStrength * 2;
	AttackDesc.iPlagueDamage = m_iPlague >> 2;
	return AttackDesc;
}

ATTACKDESC CPlayerStats::Get_PlagueAttackDesc() const
{
	ATTACKDESC AttackDesc;
	AttackDesc.ePlayerAttackType = ATTACK_NORMAL;
	AttackDesc.iDamage = m_iPlague >> 2;
	AttackDesc.iPlagueDamage = m_bShortClaw ? m_iPlague * 2 - m_iPlague / 2 : m_iPlague * 3;

	return AttackDesc;
}

ATTACKDESC CPlayerStats::Get_SkillAttackDesc(SKILLTYPE eSkillType) const
{
	ATTACKDESC AttackDesc;

	switch (eSkillType)
	{
	case AXE:
		AttackDesc.ePlayerAttackType = ATTACK_NORMAL;
		AttackDesc.iDamage = m_iStrength * 3;
		AttackDesc.iPlagueDamage = AttackDesc.iDamage;
		break;
	case HAMMER:
		AttackDesc.ePlayerAttackType = IGNORE_STANCE;
		AttackDesc.iDamage = _int(m_iStrength * 3.5f);
		AttackDesc.iPlagueDamage = AttackDesc.iDamage;
		break;
	case SPEAR:
		AttackDesc.ePlayerAttackType = IGNORE_STANCE;
		AttackDesc.iDamage = _int(m_iStrength * 3);
		AttackDesc.iPlagueDamage = AttackDesc.iDamage;
		break;
	case TWINBLADE:
		AttackDesc.ePlayerAttackType = ATTACK_NORMAL;
		AttackDesc.iDamage = _int(m_iStrength * 1.5f);
		AttackDesc.iPlagueDamage = AttackDesc.iDamage;
		break;
	}

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
