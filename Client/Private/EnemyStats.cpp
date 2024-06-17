#include "EnemyStats.h"

CEnemyStats::CEnemyStats()
{
}

void CEnemyStats::Initialize(const ENEMYDESC& EnemyDesc)
{
	m_iMaxHp = m_iHp = m_iSubHp = EnemyDesc.iMaxHp;
}

CEnemyStats* CEnemyStats::Create(const ENEMYDESC& EnemyDesc)
{
	CEnemyStats* pInstance = new CEnemyStats;

	pInstance->Initialize(EnemyDesc);

	return pInstance;
}
