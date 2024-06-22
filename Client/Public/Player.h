#pragma once

#include "Character.h"
#include "Cutscene_Actor.h"
#include "Player_Enums.h"

BEGIN(Client)

class CPlayer final : public CCharacter, public CCutscene_Actor
{
private:
	enum PLAYER_WEAPON { SABER, DAGGER, CLAW_L, CLAW_R, WEAPON_END};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	void OnEnter_Cutscene()									override;
	void OnStart_Cutscene(CUTSCENE_NUMBER eCutsceneNumber)	override;
	void OnEnd_Cutscene()									override;

	void Bind_KeyFrames()									override;

private:
	class CPlayerStat*	m_pStats = { nullptr };
public:
	CPlayerStat* Get_PlayerStat() const { 
		return m_pStats; }

private:
	CTransform*		m_pTargetTransform = { nullptr };
	_bool			m_bLockOn = { false };

	_bool			m_bCanNextState = { false };
	_bool			m_bCanRotation = { true };
	_bool			m_bInvincible = { false };

	_float4x4		m_PrevWorldMatrix; // 컷신시작전 위치
public:
	void Toggle_LockOn(CTransform* pTargetTransform = nullptr);
	_bool Is_LockOn() const { 
		return m_bLockOn; }

	_bool Can_NextState() const { 
		return m_bCanNextState; }
	_bool Can_Rotation() const { 
		return m_bCanRotation; }
	_bool Is_Invincible() const { 
		return m_bInvincible; }

	void SetState_Parried();
	void SetState_Executed(void* pArg);
	void Inactive_AllWeaponColliders();

#pragma region KEYFRAMEEVENT
public:
	void Enable_NextState() {
  		m_bCanNextState = true;
	}

	void Disable_NextState() {
		m_bCanNextState = false;
	}

	void Disable_Rotation() {
		m_bCanRotation = false;
	}

	void Enable_Rotation() {
		m_bCanRotation = true;
	}

	void Set_Invincible() {
		m_bInvincible = true;
	}
	
	void Set_Vulnerable() {
		m_bInvincible = false;
	}

private:
	void Active_Weapons();
	void InActive_Weapons();
	void Active_Claw();
	void InActive_Claw();
	
#pragma endregion

private:
	void OnCollisionEnter(CGameObject* pOther) override;

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