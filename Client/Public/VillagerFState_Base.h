#pragma once

#include "State_Base.h"
#include "Villager_F.h"

BEGIN(Client)
class CVillagerFState_Base : public CState_Base
{
protected:
	CVillagerFState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CVillagerFState_Base() = default;

public:
	HRESULT Initialize(void* pArg)					override;
	void OnState_Start(void* pArg)					override;
	void Update(_float fTimeDelta)					override;
	void OnState_End()								override;

	void OnHit(const ATTACKDESC& AttackDesc)		override;

protected:
	CVillager_F*	m_pVillager_F = { nullptr };
	CTransform*		m_pTargetTransform = { nullptr };

protected:
	virtual void Decide_State();
	void Decide_Attack();


public:
	void Free() override;
};

END

