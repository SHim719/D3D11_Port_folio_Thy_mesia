#pragma once

#include "Character.h"
#include "Cutscene_Actor.h"
#include "Player_Enums.h"

BEGIN(Client)

class CPlayer final : public CCharacter, public CCutscene_Actor
{
public:
	enum PLAYER_WEAPON { SABER, DAGGER, CLAW_L, CLAW_R
		, PW_AXE, PW_HAMMER, PW_SPEAR, PW_TWINBLADE_L, PW_TWINBLADE_R, WEAPON_END};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

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
	void Bind_KeyFrameEffects()								override;
	void Bind_KeyFrameSounds()								override;

	HRESULT Bind_ShaderResources()							override;

	void Play_FootStep();
public:
	HRESULT Reset_NaviData(LEVEL eLevel);
	void Enroll_AllColliders();
	void Set_Active_DefaultEffect(_bool bActive);

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
	SKILLTYPE				m_eNowUsingSkill = { SKILLTYPE::NONE };

	CTransform*				m_pTargetTransform = { nullptr };
	_bool					m_bLockOn = { false };

	_bool					m_bCollEnemy = { false };
	_bool					m_bCanNextState = { false };
	_bool					m_bCanNextAttack = { false };
	_bool					m_bCanRotation = { true };
	_bool					m_bCanParry = { false };
	_bool					m_bInvincible = { false };
	_bool					m_bEnableJog = { true };
	
	_float4x4				m_PrevWorldMatrix = {}; // 컷신시작전 위치

	_uint					m_iTargetNaviIdx = { 0 }; // 사다리를 다 타면 나오는 네비메쉬 인덱스
	_uint					m_iNowLadderHeight = { 0 }; // 사다리의 높이
	_uint					m_iPlayerLadderHeight = { 0 }; // 플레이어 사다리높이 - 왼쪽 발 기준(제일 낮은높이)
	_bool					m_bIsClimbStartDown = { false };

	_float					m_fHitGap = { 0.1f };
	_float					m_fHitGapAcc = { 0.f };

	wstring					m_wstrFootStepTag = { L"FootStep_Wood" };


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

	_bool Can_Jog() const {
		return m_bEnableJog;
	}

	_bool Can_Parry() const {
		return m_bCanParry;
	}

	_bool Is_Invincible() const { 
		return m_bInvincible; }

	_uint Get_NowLadderHeight() const {
		return m_iNowLadderHeight;
	}

	_uint Add_PlayerLadderHeight(_uint iHeight) {
		return m_iPlayerLadderHeight += iHeight;
	}

	_uint Get_PlayerLadderHeight() const {
		return m_iPlayerLadderHeight;
	}

	_int Get_TargetNaviIdx() const {
		return m_iTargetNaviIdx;
	}

	_bool Is_ClimbStartDown() const {
		return m_bIsClimbStartDown;
	}

	void Set_CanNextState(_bool bCanNextState) {
		m_bCanNextState = bCanNextState;
	}

	void Set_CanRotation(_bool bCanRotation) {
		m_bCanRotation = bCanRotation;
	}

	void Set_EnableJog(_bool bEnableJog) {
		m_bEnableJog = bEnableJog;
	}

	void Set_Invincible(_bool bInvincible) {
		m_bInvincible = bInvincible;
	}

	void Set_CanNextAttack(_bool bCanNextAttack) {
		m_bCanNextAttack = bCanNextAttack;
	}

	void Set_CanParry(_bool bCanParry) {
		m_bCanParry = bCanParry;
	}

	void Set_NowUsingSkill(SKILLTYPE eType) {
		m_eNowUsingSkill = eType;
	}

	void ChangeAnim_GetUp() {
		m_pModel->Change_Animation(Corvus_ArchiveGetup);
	}

	_bool Is_CollEnemy() const {
		return m_bCollEnemy;
	}

	void Toggle_LockOn(CTransform* pTargetTransform = nullptr);

	void SetState_Parried();
	void SetState_Executed(void* pArg);
	void SetState_Plunder(void* pArg);
	void SetState_ClimbStart(void* pArg);
	void SetState_FoundBeacon(void* pArg);
	void SetState_Sit(void* pArg);
	
	void Inactive_AllWeaponColliders();

	void Set_Active_DefaultWeapons(_bool bActive);
	void Set_Active_Claws(_bool bActive);

	void Set_Active_WeaponCollider(PLAYER_WEAPON eWeapon, _bool bActive);

	void Set_Active_NowPWCollider(_bool bActive);

	_int Take_Damage(const ATTACKDESC& AttackDesc)	override;

	_vector Get_Center() const override {
		return m_pTransform->Get_Position() + m_pTransform->Get_Up() * 1.5f;
	}

	const wstring& Get_FootStepTag() const {
		return m_wstrFootStepTag;
	}

	void Set_FootStepTag(const wstring& wstrFootStepTag) {
		m_wstrFootStepTag = wstrFootStepTag;
	}


private:
	void OnCollisionEnter(CGameObject* pOther)	override;
	void OnCollisionExit(CGameObject* pOther)	override;
	void ChangeToNextComboAnim();
	void Healing();

	void Calc_HitGap(_float fTimeDelta) {
		if (m_fHitGapAcc < 0.f) return;
		m_fHitGapAcc -= fTimeDelta;
	}

private:
	HRESULT Ready_Components();
	HRESULT Ready_States();
	HRESULT Ready_Weapons();
	HRESULT Ready_PlagueWeapons();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END