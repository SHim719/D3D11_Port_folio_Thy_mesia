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
	_int			m_iMaxHp;
	_int			m_iHp;
	_int			m_iSubHp;

public:
	_int Get_Hp() const { 
		return m_iHp; }


public:
	static CEnemyStats* Create(const ENEMYDESC& EnemyDesc);
};

END

