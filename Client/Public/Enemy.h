#pragma once

#include "Character.h"
#include "EnemyStats.h"

BEGIN(ENGINE)
class CBone;
END

BEGIN(Client)

class CEnemy abstract : public CCharacter
{
public:
	enum EXECUTION_TAG { DEFAULT, JOKER, TWINBLADEKNIGHT, ODUR, URD, TAG_END};

protected:
	CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy(const CEnemy& rhs);
	virtual ~CEnemy() = default;

	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	HRESULT Bind_ShaderResources()		override;
	void Decide_PassIdx()				override;
	void OnDeath()						override;
protected:
	static CGameObject* s_pTarget;

public:
	static void Set_Target(CGameObject* pTarget) {
		s_pTarget = pTarget;
	}

	static CGameObject* Get_Target() {
		return s_pTarget;
	}

protected:
	class CEnemyStats*	m_pStats = { nullptr };

public:
	CEnemyStats* Get_Stats() const {
		return m_pStats;
	}

protected:
	_bool	m_bLookTarget = { true };
	_bool	m_bCollPlayer = { false };
	_bool	m_bStunnedMarked = { false };
	_bool	m_bExecutionEnd = { false };
	_float	m_fRotRate	= { 10.f };

	_uint	m_iStunnedStateIdx = { 9999 };
	_uint	m_iStunnedStartStateIdx = { 9999 };
	_uint	m_iDeathStateIdx = { 9999 };
	_uint	m_iExecutionStateIdx = { 9999 };

	EXECUTION_TAG			m_eExecutionTag = { DEFAULT };
	SKILLTYPE				m_eSkillType = { SKILLTYPE_END };

	class CUI_EnemyBar*		m_pEnemyBar = { nullptr };
public:
	virtual void Percept_Target();

	virtual void SetState_Death();
	virtual void SetState_Executed(void* pArg);

	_bool Is_Stunned() {
		return m_iState == m_iStunnedStartStateIdx || m_iState == m_iStunnedStateIdx;
	}
	_bool Is_Death() {
		return m_iState == m_iDeathStateIdx;
	}

	_bool Is_ExecutionEnd() const {
		return m_bExecutionEnd;
	}

	CBone* Find_SpineBone();
	
	void Set_LookTarget(_bool bLookTarget) {
		m_bLookTarget = bLookTarget;
	}

	_bool Is_CollPlayer() const { 
		return m_bCollPlayer; }

	EXECUTION_TAG Get_ExecutionTag() const {
		return m_eExecutionTag; 
	}

	SKILLTYPE Get_SkillType() const {
		return m_eSkillType;
	}


protected:
	void OnCollisionEnter(CGameObject* pOther)	override;
	void OnCollisionExit(CGameObject* pOther)	override;

public:
	_int Take_Damage(const ATTACKDESC& AttackDesc)	override;
	void Active_StunnedMark();
	void InActive_StunnedMark();
	
public:
	void Free() override;
};

END