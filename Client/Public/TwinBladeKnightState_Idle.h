#pragma once

#include "TwinBladeKnightState_Base.h"

BEGIN(Client)
class CTwinBladeKnightState_Idle : public CTwinBladeKnightState_Base
{
protected:
	CTwinBladeKnightState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTwinBladeKnightState_Idle() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

public:
	static CTwinBladeKnightState_Idle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END

