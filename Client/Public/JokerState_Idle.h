#pragma once

#include "JokerState_Base.h"

BEGIN(Client)
class CJokerState_Idle final : public CJokerState_Base
{
private:
	CJokerState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CJokerState_Idle() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void OnState_End()					override;

public:
	static CJokerState_Idle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END

