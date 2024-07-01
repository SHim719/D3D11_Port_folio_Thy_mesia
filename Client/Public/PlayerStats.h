#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
class CPlayerStats : public CBase
{
private:
	CPlayerStats();
	virtual ~CPlayerStats() = default;

private: 
	_bool			m_bAttackUpgraded = { true };
	_bool			m_bShortClaw = { true };

	_int			m_iHp = { 300 };
	_int			m_iMaxHp = { 300 };

	_int			m_iMp = { 150 };
	_int			m_iMaxMp = { 150 };

	_int			m_iStrength = { 10 };
	_int			m_iPlague = { 10 };
	_int			m_iVitality = { 10 };


public:
	void Upgrade_Attack() { 
		m_bAttackUpgraded = true; }

	_uint Get_MaxAttackCnt() const {
		return m_bAttackUpgraded ? 5 : 3;
	}

	_bool Is_ShortClaw() const {
		return m_bShortClaw;
	}


	_int Increase_Hp(_int iHp);
	void Increase_MaxHp(_int iHp);
	void Increase_Mp(_int iMp);
	void Increase_MaxMp(_int iMp);

	ATTACKDESC Get_NormalAttackDesc() const;
	ATTACKDESC Get_PlagueAttackDesc() const;

	
public:
	void Add_Observer(class CUI* pUI);

private:
	void Broadcast_Update_Hp()	const;
	void Broadcast_Update_Mp()	const;
	
private:
	list<class CUI*>	m_ObserverUIs;


public:
	static CPlayerStats* Create();
	void Free() override;
};
END

