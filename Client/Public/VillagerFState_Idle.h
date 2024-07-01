#pragma once

#include "VillagerFState_Base.h"

BEGIN(Client)
class CVillagerFState_Idle : public CVillagerFState_Base
{
protected:
	CVillagerFState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CVillagerFState_Idle() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void OnState_End()					override;

public:
	static CVillagerFState_Idle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END

