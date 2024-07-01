#pragma once

#include "Character.h"
#include "Cutscene_Actor.h"
#include "Player_Enums.h"

BEGIN(Client)

class CPlayer final : public CCharacter, public CCutscene_Actor
{
public:
	enum PLAYER_WEAPON { SABER, DAGGER, CLAW_L, CLAW_R, WEAPON_END};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

private:
	HRESULT Initialize_Prototype()			override;
	HRESULT Initialize(void* pArg)			override;
	void PriorityTick(_float fTimeDelta)	override;
	void Tick(_float fTimeDelta)			override;
	void LateTick(_float fTimeDelta)		override;
	HRESULT Render()						override;

	void OnEnter_Cutscene()									override;
	void OnStart_Cutscene(CUTSCENE_NUMBER eCutsceneNumber)	override;
	void OnEnd_Cutscene()									override;

	void Bind_KeyFrames()									override;

private:
	class CPlayerStats*	m_pStats = { nullptr };

public:
	CPlayerStats* Get_PlayerStats() const {
		return m_pStats;
	}

private:
	_float				m_fExecutionDist = { 4.5f };
	class CEnemy*		m_pExecutionTarget = { nullptr };

	list<class CEnemy*> m_StunnedEnemies;
private:
	void Update_CanExecutionEnemy();

public:
	void Add_StunnedEnemy(class CEnemy* pEnemy) {
		m_StunnedEnemies.push_back(pEnemy);
	}

	class CEnemy* Get_ExecutionEnemy() const {
		return m_pExecutionTarget;
	}


private:
	CTransform*		m_pTargetTransform = { nullptr };
	_bool			m_bLockOn = { false };

	_bool			m_bCanNextState = { false };
	_bool			m_bCanNextAttack = { false };
	_bool			m_bCanRotation = { true };
	_bool			m_bInvincible = { false };
	
	_float4x4		m_PrevWorldMatrix; // 컷신시작전 위치

public:
	_bool Is_LockOn() const { 
		return m_bLockOn; }

	_bool Can_NextState() const { 
		return m_bCanNextState; }

	_bool Can_Rotation() const { 
		return m_bCanRotation; }

	_bool Can_NextAttack() const {
		return m_bCanNextAttack;
	}
	_bool Is_Invincible() const { 
		return m_bInvincible; }

	void Set_CanNextState(_bool bCanNextState) {
		m_bCanNextState = bCanNextState;
	}

	void Set_CanRotation(_bool bCanRotation) {
		m_bCanRotation = bCanRotation;
	}

	void Set_Invincible(_bool bInvincible) {
		m_bInvincible = bInvincible;
	}

	void Set_CanNextAttack(_bool bCanNextAttack) {
		m_bCanNextAttack = bCanNextAttack;
	}

	void Toggle_LockOn(CTransform* pTargetTransform = nullptr);
	void SetState_Parried();
	void SetState_Executed(void* pArg);
	void Inactive_AllWeaponColliders();
	void Set_Active_DefaultWeapons(_bool bActive);
	void Set_Active_Claws(_bool bActive);

private:
	void OnCollisionEnter(CGameObject* pOther)	override;
	
public:
	_int Take_Damage(const ATTACKDESC& AttackDesc)	override;


private:
	HRESULT Ready_Components();
	HRESULT Ready_States();
	HRESULT Ready_Weapons();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END