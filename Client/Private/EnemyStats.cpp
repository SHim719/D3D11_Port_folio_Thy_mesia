#include "EnemyStats.h"

#include "UI_EnemyBar.h"
#include "UI_BossBar.h"

CEnemyStats::CEnemyStats()
{
}

void CEnemyStats::Initialize(const ENEMYDESC& EnemyDesc)
{
	m_wstrName = EnemyDesc.wstrEnemyName;
	m_iMaxHp = m_iHp = m_iMp = EnemyDesc.iMaxHp;
	m_bIsBoss = EnemyDesc.bIsBoss;
}

void CEnemyStats::Set_EnemyBar(CUI_EnemyBar* pEnemyBar)
{
	m_pEnemyBar = pEnemyBar;
	Safe_AddRef(m_pEnemyBar);
}

void CEnemyStats::Set_BossBar(CUI_BossBar* pBossBar)
{
	m_pBossBar = pBossBar;
	Safe_AddRef(m_pBossBar);
}

void CEnemyStats::Increase_Hp(_int iHp)
{
	m_iHp += iHp;
	m_iHp = clamp(m_iHp, 0, m_iMaxHp);

	if (m_bIsBoss)
		m_pBossBar->Update_EnemyHp(m_iHp);
	else
		m_pEnemyBar->Update_EnemyHp(m_iHp);
}

_int CEnemyStats::Increase_Mp(_int iMp)
{
	m_iMp += iMp;
	m_iMp = clamp(m_iMp, m_iHp, m_iMaxHp);

	if (m_bIsBoss)
		m_pBossBar->Update_EnemyMp(m_iMp);
	else
		m_pEnemyBar->Update_EnemyMp(m_iMp);

	return m_iMp;
}


CEnemyStats* CEnemyStats::Create(const ENEMYDESC& EnemyDesc)
{
	CEnemyStats* pInstance = new CEnemyStats;

	pInstance->Initialize(EnemyDesc);

	return pInstance;
}

void CEnemyStats::Free()
{
	Safe_Release(m_pEnemyBar);
	Safe_Release(m_pBossBar);
}
