#pragma once

#include "HalberdKnightState_Base.h"

BEGIN(Client)
class CHalberdKnightState_Hit final : public CHalberdKnightState_Base
{
private:
	CHalberdKnightState_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CHalberdKnightState_Hit() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

private:
	void OnHit(const ATTACKDESC& AttackDesc) override;

private:
	_int	m_iHitCount = { 0 };
	_int	m_iMaxHitCount = { 15 };


public:
	static CHalberdKnightState_Hit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END

