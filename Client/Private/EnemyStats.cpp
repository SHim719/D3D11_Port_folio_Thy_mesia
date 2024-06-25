#include "EnemyStats.h"

#include "UI.h"

CEnemyStats::CEnemyStats()
{
}

void CEnemyStats::Initialize(const ENEMYDESC& EnemyDesc)
{
	m_wstrName = EnemyDesc.wstrEnemyName;
	m_iMaxHp = m_iHp = m_iMp = EnemyDesc.iMaxHp;
}

void CEnemyStats::Increase_Hp(_int iHp)
{
	m_iHp += iHp;
	m_iHp = clamp(m_iHp, 0, m_iMaxHp);
	Broadcast_Update_Hp();
}

void CEnemyStats::Increase_Mp(_int iMp)
{
	m_iMp += iMp;
	m_iMp = clamp(m_iMp, 0, m_iMaxHp);
	Broadcast_Update_Mp();
}

void CEnemyStats::Add_Observer(CUI* pUI)
{
	m_ObserverUIs.push_back(pUI);
	Safe_AddRef(pUI);

	Broadcast_Update_Hp();
	Broadcast_Update_Mp();
}

void CEnemyStats::Broadcast_Update_Hp()
{
	for (auto& pUI : m_ObserverUIs)
		pUI->Update_EnemyHp(m_iHp);
}

void CEnemyStats::Broadcast_Update_Mp()
{
	for (auto& pUI : m_ObserverUIs)
		pUI->Update_EnemyMp(m_iMp);
}

CEnemyStats* CEnemyStats::Create(const ENEMYDESC& EnemyDesc)
{
	CEnemyStats* pInstance = new CEnemyStats;

	pInstance->Initialize(EnemyDesc);

	return pInstance;
}

void CEnemyStats::Free()
{
	for (auto& pUI : m_ObserverUIs)
		Safe_Release(pUI);

	m_ObserverUIs.clear();
}
