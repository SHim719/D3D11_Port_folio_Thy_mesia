#pragma once

#include "VillagerMState_Base.h"

BEGIN(Client)
class CVillagerMState_Idle final : public CVillagerMState_Base
{
private:
	CVillagerMState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CVillagerMState_Idle() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void OnState_End()					override;

public:
	static CVillagerMState_Idle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END

