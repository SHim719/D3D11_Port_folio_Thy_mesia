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
	_bool			m_bShortClaw = { false };
	_bool			m_bObtainKey = { false };

	_int			m_iLevel = { 1 };
	_int			m_iHp = { 300 };
	_int			m_iMaxHp = { 300 };

	_int			m_iStrength = { 10 };
	_int			m_iPlague = { 10 };
	_int			m_iVitality = { 10 };

	_int			m_iSoulCount = { 500 };

	SKILLTYPE		m_ePlunderSkill = { NONE };
	SKILLTYPE		m_eUsingSkill = { NONE };

	_bool			m_bSkillActived[SKILLTYPE::SKILLTYPE_END] = {};
public:
	SKILLTYPE Get_PlunderSkillType() const {
		return m_ePlunderSkill;
	}

	SKILLTYPE Get_UsingSkillType() const {
		return m_eUsingSkill;
	}

	_bool Is_ShortClaw() const {
		return m_bShortClaw;
	}

	_bool Is_SkillActived(SKILLTYPE eSkillType) const {
		return m_bSkillActived[eSkillType];
	}

	_bool Have_Key() const {
		return m_bObtainKey;
	}

	_int Get_Strength() const {
		return m_iStrength;
	}

	_int Get_Plague() const {
		return m_iPlague;
	}

	_int Get_Vitality() const {
		return m_iVitality;
	}

	_int Get_SoulCount() const {
		return m_iSoulCount;
	}

	_int Get_PlayerLevel() const {
		return m_iLevel;
	}

	_int Get_MaxHp() const {
		return m_iMaxHp;
	}


	void Set_Strength(_uint iStrength) {
		m_iStrength = iStrength;
	}

	void Set_Plague(_uint iPlague) {
		m_iPlague = iPlague;
	}

	void Set_Vitality(_uint iVitality) {
		m_iVitality = iVitality;
	}

	void Set_SoulCount(_int iSoulCount) {
		m_iSoulCount = iSoulCount;
	}

	void Set_PlayerLevel(_int iLevel) {
		m_iLevel = iLevel;
	}

	void Set_MaxHp(_int iMaxHp) {
		m_iMaxHp = iMaxHp;
	}

	void Set_UseShortClaw(_bool bUseShortClaw) {
		m_bShortClaw = bUseShortClaw;
	}

	void Set_UsingSkill(SKILLTYPE eSkill) {
		m_eUsingSkill = eSkill;
	}

	void Obtain_Key() {
		m_bObtainKey = true;
	}

	void Active_Skill(SKILLTYPE eSkill) {
		m_bSkillActived[eSkill] = true;
	}

public:
	void Update_PlunderSkill(const SKILLTYPE ePlunderSkill);

	_int Increase_Hp(_int iHp);
	void Increase_MaxHp(_int iHp);

	void SetHp_Full();

	void Increase_SoulCount(_int iSoul);

	ATTACKDESC Get_NormalAttackDesc() const;
	ATTACKDESC Get_PlagueAttackDesc() const;

private:
	class CUI_PlayerDefault*	m_pPlayerUI = { nullptr };

public:
	void Set_PlayerDefaultUI(CUI_PlayerDefault* pUI);
		
public:
	static CPlayerStats* Create();
	void Free() override;
};
END

