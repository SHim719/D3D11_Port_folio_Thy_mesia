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

	_float			m_fHp = { 300.f };
	_float			m_fMaxHp = { 300.f };
public:
	void Upgrade_Attack() { 
		m_bAttackUpgraded = true; }

	_uint Get_MaxAttackCnt() const {
		return m_bAttackUpgraded ? 5 : 3;
	}

	void Increase_Hp(_float fHp);


	void Increase_MaxHp(_float fHp) {
		m_fMaxHp += fHp;
		Broadcast_Update_Hp();
	}


public:
	void Add_Observer(class CUI* pUI);

private:
	void Broadcast_Update_Hp();
	void Broadcast_Update_Mp();
	
private:
	list<class CUI*>	m_ObserverUIs;


public:
	static CPlayerStat* Create();
	void Free() override;
};
END

