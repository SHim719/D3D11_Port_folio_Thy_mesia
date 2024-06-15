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
	virtual void Hit(void* pArg);

protected:
	vector<class CWeapon*>		m_Weapons;

protected:
	CShader*	m_pShader = { nullptr };
	CModel*		m_pModel = { nullptr };
	CCollider*	m_pCollider = { nullptr };
	CCollider*	m_pHitBoxCollider = { nullptr };

public:
	void Free() override;
};

END