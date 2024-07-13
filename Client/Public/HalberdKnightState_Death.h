#pragma once

#include "HalberdKnightState_Base.h"

BEGIN(Client)
class CHalberdKnightState_Death final : public CHalberdKnightState_Base
{
private:
	CHalberdKnightState_Death(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CHalberdKnightState_Death() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

private:
	void OnHit(const ATTACKDESC& AttackDesc) override;

public:
	static CHalberdKnightState_Death* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END



