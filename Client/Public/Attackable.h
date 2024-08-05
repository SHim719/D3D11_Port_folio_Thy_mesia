#pragma once

#include "Client_Defines.h"
#include "Component.h"

BEGIN(Client)

class CAttackable final : public CComponent
{
private:
	CAttackable();
	CAttackable(const CAttackable& rhs);
	virtual ~CAttackable() = default;

private:
	HRESULT Initialize_Prototype()	override;
	HRESULT Initialize(void* pArg)	override;

private:
	ATTACKDESC		m_tAttackDesc;
	
public:
	void Set_AttackDesc(const ATTACKDESC& AttackDesc) {
		m_tAttackDesc = AttackDesc;
	}

	const ATTACKDESC& Get_AttackDesc() const {
		return m_tAttackDesc;
	}

public:
	static CAttackable* Create();
	CComponent* Clone(void* pArg)	override;
	void Free()						override;
};

END
