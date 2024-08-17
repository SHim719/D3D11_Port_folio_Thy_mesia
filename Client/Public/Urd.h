#pragma once

#include "Enemy.h"
#include "Urd_Enums.h"
#include "Cutscene_Actor.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class CUrd final : public CEnemy, public CCutscene_Actor
{
public:
	enum URD_WEAPON { SWORD, WEAPON_END };

private:
	CUrd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUrd(const CUrd& rhs);
	virtual ~CUrd() = default;

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
	void Bind_KeyFrameEffects()								override;

private:
	CBone*				m_pSwapBone = { nullptr };
	_float				m_fRotRate = { 8.f };

	_float4x4			m_InitWorldMatrix = {};
	_bool				m_bPhase2 = { false };
	_bool				m_bInvincible = { false };
private:
	vector<class CUrd_Weapon*>  m_Urd_Weapons;
	size_t						m_iNowWeaponIdx = { 0 };
	size_t						m_iNumActivatedWeapons = { 0 };

	_int						m_iStepCount = { 0 };

	class CUrd_MagicCircle*		m_pMagicCircle = { nullptr };

private:
	void Active_Ultimate_Skill();

public:
	void Swap_Bone();
	void Active_UrdWeapon();
	void Active_MagicCircle(_bool bUltimate);
	void Release_UrdWeapon(_bool bThrow);

	void SetState_Death()				override;
	void SetState_Executed(void* pArg)	override;

	void Decide_State();

	void Active_Phase2() {
		m_bPhase2 = true;
	}

	_bool Is_Phase2() {
		return m_bPhase2;
	}

	void Set_Invincible(_bool bInvincible) {
		m_bInvincible = bInvincible;
	}

	_bool Is_Invincible() {
		return m_bInvincible;
	}

	size_t Get_NumActivatedWeapons() {
		return m_iNumActivatedWeapons;
	}

	size_t Get_NumWeapons() {
		return m_Urd_Weapons.size();
	}

	_bool Can_EnableSkill() {
		return m_iNumActivatedWeapons < m_Urd_Weapons.size();
	}

	_bool Is_PrevState_Step() const
	{
		return m_iState == UrdState::State_Step_F || m_iState == UrdState::State_Step_FLR ||
			m_iState == UrdState::State_Step_LR;
	}

	_bool Can_StepState() const {
		return m_iStepCount < 2;
	}

	void Reset_StepCount() {
		m_iStepCount = 0;
	}

	void Add_StepCount()  {
		++m_iStepCount;
	}

	void Resize_WeaponCollider(_bool bOrigin);

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Ready_States();
	HRESULT Ready_Weapons();
	HRESULT Ready_Stats();
	HRESULT Ready_Light();

public:
	static CUrd* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END