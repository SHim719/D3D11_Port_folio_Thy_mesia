#pragma once

#include "State_Base.h"
#include "Villager_M.h"

BEGIN(Client)
class CVillagerMState_Base : public CState_Base
{
protected:
	CVillagerMState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CVillagerMState_Base() = default;

public:
	HRESULT Initialize(void* pArg)					override;
	void OnState_Start(void* pArg)					override;
	void Update(_float fTimeDelta)					override;
	void OnState_End()								override;

	void OnHit(const ATTACKDESC& AttackDesc)		override;

protected:
	CVillager_M*	m_pVillager_M = { nullptr };
	CTransform*		m_pTargetTransform = { nullptr };

protected:
	virtual void Decide_State();

public:
	void Free() override;
};

END

