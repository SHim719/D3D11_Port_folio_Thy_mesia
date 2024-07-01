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

	if (false == m_bHit)
	{
		m_bHit = true;
 		Broadcast_Hit();
	}
}

_int CEnemyStats::Increase_Mp(_int iMp)
{
	m_iMp += iMp;
	m_iMp = clamp(m_iMp, m_iHp, m_iMaxHp);
	Broadcast_Update_Mp();

	return m_iMp;
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

void CEnemyStats::Broadcast_Hit()
{
	for (auto& pUI : m_ObserverUIs)
		pUI->Enemy_FirstHit();
}

void CEnemyStats::Broadcast_Death()
{
	for (auto& pUI : m_ObserverUIs)
		pUI->Broadcast_Death();
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
