#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Client)

class CEffect_Hit: public CGameObject
{
public:
	typedef struct tagEffect_HitDesc
	{
		CCollider::COLLIDERDESC*	pColliderDesc = nullptr;
		ATTACKDESC					AttackDesc = {};
		_float4x4					SpawnMatrix = {};
		class CEffect_Group*		pEffect_Group = nullptr;
	} EFFECT_HITDESC;

protected:
	CEffect_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Hit(const CEffect_Hit& rhs);
	virtual ~CEffect_Hit() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;

protected:
	CAttackable*		m_pAttackable = { nullptr };

public:
	void Set_AttackDesc(const ATTACKDESC& Desc) {
		m_pAttackable->Set_AttackDesc(Desc);
	}

	const ATTACKDESC& Get_AttackDesc() const {
		return m_pAttackable->Get_AttackDesc();
	}

private:
	class CEffect_Group*	m_pEffect_Group = { nullptr };
	CCollider*				m_pCollider = { nullptr };

protected:
	HRESULT Ready_Components(void* pArg);

public:
	static CEffect_Hit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	void Free() override;
};

END