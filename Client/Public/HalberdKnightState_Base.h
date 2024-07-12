#pragma once

#include "State_Base.h"
#include "HalberdKnight.h"

BEGIN(Client)
class CHalberdKnightState_Base : public CState_Base
{
protected:
	CHalberdKnightState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CHalberdKnightState_Base() = default;

public:
	HRESULT Initialize(void* pArg)					override;
	void OnState_Start(void* pArg)					override;
	void Update(_float fTimeDelta)					override;
	void OnState_End()								override;

	void OnHit(const ATTACKDESC& AttackDesc)		override;

protected:
	CHalberdKnight* m_pHalberdKnight = { nullptr };
	CTransform*		m_pTargetTransform = { nullptr };

protected:
	virtual void Decide_State();
	void Decide_Attack();


public:
	void Free() override;
};

END

