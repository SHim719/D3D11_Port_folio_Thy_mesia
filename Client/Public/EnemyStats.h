#pragma once

#include "Client_Defines.h"
#include "Base.h"


BEGIN(Client)

class CEnemyStats : public CBase
{
private:
	CEnemyStats();
	virtual ~CEnemyStats() = default;

	void Initialize(const ENEMYDESC& EnemyDesc);

private:
	wstring			m_wstrName = { L"" };
	_bool			m_bIsBoss = { false };

	_int			m_iMaxHp = { 0 };
	_int			m_iHp = { 0 };
	_int			m_iMp = { 0 };

private:
	class CUI_EnemyBar*	m_pEnemyBar = { nullptr };
	class CUI_BossBar*	m_pBossBar = { nullptr };

public:
	void Set_EnemyBar(CUI_EnemyBar* pEnemyBar);
	void Set_BossBar(CUI_BossBar* pBossBar);

public:
	void Increase_Hp(_int iHp);
	_int Increase_Mp(_int iMp);

public:
	const wstring& Get_Name() const {
		return m_wstrName;
	}

	_int Get_MaxHp() const {
		return m_iMaxHp;
	}

	_bool Is_Hit() const {
		return m_iMaxHp != m_iHp;
	}


public:
	static CEnemyStats* Create(const ENEMYDESC& EnemyDesc);
	void Free() override;
};

END

