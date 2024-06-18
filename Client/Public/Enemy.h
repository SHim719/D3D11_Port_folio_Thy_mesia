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
	class CEnemyStats* m_pStats = { nullptr };

public:
	CEnemyStats* Get_Stats() const { 
		return m_pStats; }

protected:
	_bool m_bLookTarget = { true };
	_bool m_bCollPlayer = { false };

public:
	void Enable_LookTarget() {
		m_bLookTarget = true;
	}
	void Disable_LookTarget() {
		m_bLookTarget = false;
	}

	_bool Is_CollPlayer() const { return m_bCollPlayer; }

protected:
	void OnCollisionEnter(CGameObject* pOther)	override;
	void OnCollisionExit(CGameObject* pOther)	override;
	

public:
	void Free() override;
};

END