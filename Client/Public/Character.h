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
	virtual _int Take_Damage(const ATTACKDESC& AttackDesc);

protected:
	_bool						m_bStanced = { false };
	_bool						m_bAdjustNaviY = { true };
	
public:
	_bool Is_Stanced() const {
		return m_bStanced;
	}
	void Set_Stanced(_bool bStanced) {
		m_bStanced = bStanced;
	}

protected:
	vector<class CWeapon*>		m_Weapons;

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