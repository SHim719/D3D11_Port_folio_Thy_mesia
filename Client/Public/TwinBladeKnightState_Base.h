#pragma once

#include "State_Base.h"
#include "TwinBladeKnight.h"

BEGIN(Client)
class CTwinBladeKnightState_Base : public CState_Base
{
protected:
	CTwinBladeKnightState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTwinBladeKnightState_Base() = default;

public:
	HRESULT Initialize(void* pArg)					override;
	void OnState_Start(void* pArg)					override;
	void Update(_float fTimeDelta)					override;
	void OnState_End()								override;

	void OnHit(const ATTACKDESC& AttackDesc)		override;
	void Play_HitSound(_float fVolume = 0.5f)		override;
protected:
	CTwinBladeKnight*	m_pTwinBladeKnight = { nullptr };
	CTransform*			m_pTargetTransform = { nullptr };

protected:
	void Decide_State();
	void Decide_Attack();

public:
	void Free() override;
};

END

