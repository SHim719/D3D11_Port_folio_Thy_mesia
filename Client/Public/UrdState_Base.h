#pragma once

#include "State_Base.h"
#include "Urd.h"

BEGIN(Client)
class CUrdState_Base : public CState_Base
{
protected:
	CUrdState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUrdState_Base() = default;

public:
	HRESULT Initialize(void* pArg)					override;
	void OnState_Start(void* pArg)					override;
	void Update(_float fTimeDelta)					override;
	void OnState_End()								override;

	void OnHit(const ATTACKDESC& AttackDesc)		override;

public:
	virtual void Decide_State();
	void Decide_Step();
	void Decide_Attack();

protected:
	CUrd*			m_pUrd = { nullptr };
	CTransform*		m_pTargetTransform = { nullptr };

	URDSTEP_DIR		m_eDir = { DIR_END };

public:
	void Free() override;
};

END

