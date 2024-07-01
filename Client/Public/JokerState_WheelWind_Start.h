#pragma once

#include "JokerState_Base.h"

BEGIN(Client)
class CJokerState_WheelWind_Start final : public CJokerState_Base
{
private:
	CJokerState_WheelWind_Start(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CJokerState_WheelWind_Start() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

public:
	static CJokerState_WheelWind_Start* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END


