#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CCharacter abstract : public CGameObject
{
protected:
	CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;

	virtual void Bind_KeyFrames();

protected:
	vector<class CState_Base*>	m_States;
	_uint						m_iState = { 0 };
	_uint						m_iPrevState = { 0 };

public:
	void Change_State(_uint eState, void* pArg = nullptr);
	_uint Get_PrevState() const { return m_iPrevState; }

public:
	virtual void Hit(void* pArg);

protected:
	_bool						m_bStanced = { false };

public:
	void Enable_Stanced() {
		m_bStanced = true;
	}

	void Disable_Stanced() {
		m_bStanced = false;
	}

protected:
	vector<class CWeapon*>		m_Weapons;

	size_t	m_iAttackIdx = { 0 };
public:
	const ATTACKDESC& Get_NowAttackDesc(size_t iIdx);

	size_t Get_AttackIdx() const {
		return m_iAttackIdx;
	}

	void Add_AttackIdx() {
		m_iAttackIdx++;
	}

	void Reset_AttackIdx() {
		m_iAttackIdx = 0;
	}

	void Active_Colliders() {
		m_pCollider->Set_Active(true);
		m_pHitBoxCollider->Set_Active(true);
	}

	void Inactive_Colliders() {
		m_pCollider->Set_Active(false);
		m_pHitBoxCollider->Set_Active(false);
	}

protected:
	void Compute_YPos();

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