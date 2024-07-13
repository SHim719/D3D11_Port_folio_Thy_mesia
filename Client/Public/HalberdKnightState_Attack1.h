#pragma once

#include "HalberdKnightState_Base.h"

BEGIN(Client)
class CHalberdKnightState_Attack1 final : public CHalberdKnightState_Base
{
private:
	CHalberdKnightState_Attack1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CHalberdKnightState_Attack1() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

	void Init_AttackDesc()				override;

public:
	static CHalberdKnightState_Attack1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END



