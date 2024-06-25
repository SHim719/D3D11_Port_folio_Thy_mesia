#include "PlayerStats.h"
#include "UI.h"


CPlayerStats::CPlayerStats()
{
}

_int CPlayerStats::Increase_Hp(_int iHp)
{
	m_iHp += iHp;	
	m_iHp = clamp(m_iHp, 0, m_iMaxHp);
	Broadcast_Update_Hp();
	return m_iHp;
}

void CPlayerStats::Increase_MaxHp(_int iHp)
{
	m_iMaxHp += iHp;
	Broadcast_Update_Hp();
}

void CPlayerStats::Increase_Mp(_int iMp)
{
	m_iMp += iMp;
	m_iMp = clamp(m_iMp, 0, m_iMaxMp);
	Broadcast_Update_Mp();
}

void CPlayerStats::Increase_MaxMp(_int iMp)
{
	m_iMaxHp += iMp;
	Broadcast_Update_Mp();
}

ATTACKDESC CPlayerStats::Get_NormalAttackDesc() const
{
	ATTACKDESC AttackDesc;
	AttackDesc.iDamage = m_iStrength * 2;
	AttackDesc.iPlagueDamage = m_iStrength;

	return AttackDesc;
}

ATTACKDESC CPlayerStats::Get_PlagueAttackDesc() const
{
	ATTACKDESC AttackDesc;
	AttackDesc.iDamage = 0;
	AttackDesc.iPlagueDamage = m_iPlague * 3;

	return AttackDesc;
}

void CPlayerStats::Add_Observer(CUI* pUI)
{
	m_ObserverUIs.emplace_back(pUI);
	Safe_AddRef(pUI);

	Broadcast_Update_Hp();
	Broadcast_Update_Mp();
}

void CPlayerStats::Broadcast_Update_Hp() const
{
	for (auto& pUI : m_ObserverUIs)
		pUI->Update_PlayerHp(m_iMaxHp, m_iHp);
}

void CPlayerStats::Broadcast_Update_Mp() const
{
	for (auto& pUI : m_ObserverUIs)
		pUI->Update_PlayerMp(m_iMaxMp, m_iMp);
}

CPlayerStats* CPlayerStats::Create()
{
	return new CPlayerStats;
}

void CPlayerStats::Free()
{
	for (auto& pUI : m_ObserverUIs)
		Safe_Release(pUI);

	m_ObserverUIs.clear();
}
