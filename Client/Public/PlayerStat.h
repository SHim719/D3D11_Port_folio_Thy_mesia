#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
class CPlayerStat : public CBase
{
private:
	CPlayerStat();
	virtual ~CPlayerStat() = default;

private: 
	_bool			m_bAttackUpgraded = true;

	_int			m_iHp;
	_int			m_iMaxHp;
public:
	void Upgrade_Attack() { 
		m_bAttackUpgraded = true; }

	_uint Get_MaxAttackCnt() const {
		return m_bAttackUpgraded ? 5 : 3;
	}


public:
	static CPlayerStat* Create();

};
END

