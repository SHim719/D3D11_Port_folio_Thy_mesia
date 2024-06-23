#pragma once

#include "OdurState_Base.h"

BEGIN(Client)
class COdurState_ExecutionDisappear : public COdurState_Base
{
protected:
	COdurState_ExecutionDisappear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COdurState_ExecutionDisappear() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void OnGoing(_float fTimeDelta)		override;
	void OnState_End()					override;

public:
	static COdurState_ExecutionDisappear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END
