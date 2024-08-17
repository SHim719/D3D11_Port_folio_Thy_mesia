#pragma once

#include "State_Base.h"
#include "Joker.h"

BEGIN(Client)
class CJokerState_Base : public CState_Base
{
protected:
	CJokerState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CJokerState_Base() = default;

public:
	HRESULT Initialize(void* pArg)					override;
	void OnState_Start(void* pArg)					override;
	void Update(_float fTimeDelta)					override;
	void OnState_End()								override;

	void OnHit(const ATTACKDESC& AttackDesc)		override;
protected:
	CJoker*		m_pJoker = { nullptr };
	CTransform* m_pTargetTransform = { nullptr };

public:
	virtual void Decide_State();
	void Decide_Attack();

public:
	void Free() override;
};

END

