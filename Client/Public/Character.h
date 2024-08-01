#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "GameEffect.h"

BEGIN(Client)

class CCharacter abstract : public CGameObject
{
protected:
	CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;

	virtual void Bind_KeyFrames();
	virtual void Bind_KeyFrameEffects();

protected:
	vector<class CState_Base*>	m_States;
	_uint						m_iState = { 0 };
	_uint						m_iPrevState = { 0 };

public:
	void Change_State(_uint eState, void* pArg = nullptr);
	_uint Get_PrevState() const { return m_iPrevState; }
	_uint Get_NowState() const { return m_iState; }

	void Change_Navigation(CNavigation* pNavigation);
	void Set_NaviIdx(_uint iNaviIdx) {
		m_pNavigation->Set_CurrentIdx(iNaviIdx);
	}

	CGameEffect::EFFECTSPAWNDESC Get_EffectSpawnDesc() const {
		return m_tEffectSpawnDesc;
	}
public:
	void Hit(const ATTACKDESC& AttackDesc);
	virtual _int Take_Damage(const ATTACKDESC& AttackDesc);

protected:
	_bool							m_bStanced = { false };
	_bool							m_bAdjustNaviY = { true };

	CGameEffect::EFFECTSPAWNDESC	m_tEffectSpawnDesc = {};
public:
	_bool Is_Stanced() const {
		return m_bStanced;
	}
	void Set_Stanced(_bool bStanced) {
		m_bStanced = bStanced;
	}

	void Active_Dissolve();
protected:
	vector<class CWeapon*>		m_Weapons;

protected:
	void Tick_Weapons(_float fTimeDelta);
	void LateTick_Weapons(_float fTimeDelta);

public:
	void Update_AttackDesc();

	void Set_Active_Weapon(_uint iWeaponIdx, _bool bActive);

	void Set_Active_Colliders(_bool bActive) {
		m_pCollider->Set_Active(bActive);
		m_pHitBoxCollider->Set_Active(bActive);
	}

public:
	void Set_Adjust_NaviY(_bool b) {
		m_bAdjustNaviY = b;
	}

protected:
	void Compute_YPos();
	void Update_Colliders();

protected:
	_bool			m_bRimLight = { false };
	RIMLIGHTDESC	m_tRimLightDesc = {};

protected:
	CShader*		m_pShader = { nullptr };
	CModel*			m_pModel = { nullptr };
	CCollider*		m_pCollider = { nullptr };
	CCollider*		m_pHitBoxCollider = { nullptr };
	CNavigation*	m_pNavigation = { nullptr };

public:
	void Free() override;
};

END