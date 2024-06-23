#include "PlayerStat.h"
#include "UI.h"


CPlayerStat::CPlayerStat()
{
}

void CPlayerStat::Increase_Hp(_float fHp)
{
	m_fHp += fHp;	
	m_fHp = clamp(m_fHp, 0.f, m_fMaxHp);
	Broadcast_Update_Hp();
}

void CPlayerStat::Add_Observer(CUI* pUI)
{
	m_ObserverUIs.push_back(pUI);
	Safe_AddRef(pUI);
}

void CPlayerStat::Broadcast_Update_Hp()
{
	for (auto& pUI : m_ObserverUIs)
		pUI->Update_PlayerHp(m_fMaxHp, m_fHp);
}

void CPlayerStat::Broadcast_Update_Mp()
{
}


CPlayerStat* CPlayerStat::Create()
{
	return new CPlayerStat;
}

void CPlayerStat::Free()
{
	for (auto& pUI : m_ObserverUIs)
		Safe_Release(pUI);

	m_ObserverUIs.clear();
}
