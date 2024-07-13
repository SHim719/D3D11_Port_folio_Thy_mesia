#pragma once

#include "TwinBladeKnightState_Base.h"

BEGIN(Client)
class CTwinBladeKnightState_ComboB final : public CTwinBladeKnightState_Base
{
private:
	CTwinBladeKnightState_ComboB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTwinBladeKnightState_ComboB() = default;

private:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

	void Init_AttackDesc()				override;

public:
	static CTwinBladeKnightState_ComboB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END

