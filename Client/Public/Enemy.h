#pragma once

#include "Character.h"
#include "EnemyStats.h"

BEGIN(Client)

class CEnemy abstract : public CCharacter
{
protected:
	CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy(const CEnemy& rhs);
	virtual ~CEnemy() = default;

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

protected:
	_bool m_bLookTarget = { true };
	_bool m_bCollPlayer = { false };

public:
	void Set_LookTarget(_bool bLookTarget) {
		m_bLookTarget = bLookTarget;
	}

	_bool Is_CollPlayer() const { return m_bCollPlayer; }

protected:
	void OnCollisionEnter(CGameObject* pOther)	override;
	void OnCollisionExit(CGameObject* pOther)	override;
	
public:
	void Free() override;
};

END