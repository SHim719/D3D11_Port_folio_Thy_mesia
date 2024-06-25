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

	_int			m_iMaxHp = { 0 };
	_int			m_iHp = { 0 };
	_int			m_iMp = { 0 };

private:
	void Increase_Hp(_int iHp);
	void Increase_Mp(_int iMp);
private:
	void Broadcast_Update_Hp();
	void Broadcast_Update_Mp();

public:
	void Add_Observer(class CUI* pUI);

private:
	list<class CUI*>	m_ObserverUIs;

public:
	const wstring& Get_Name() const {
		return m_wstrName;
	}

	_int Get_MaxHp() const {
		return m_iMaxHp;
	}

public:
	static CEnemyStats* Create(const ENEMYDESC& EnemyDesc);
	void Free() override;
};

END

