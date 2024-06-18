#pragma once

#include "OdurState_Base.h"

BEGIN(Client)
class COdurState_ReadyExecution : public COdurState_Base
{
protected:
	COdurState_ReadyExecution(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COdurState_ReadyExecution() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void OnGoing(_float fTimeDelta)		override;
	void OnState_End()					override;

private:
	void ChangeState_Execution();


public:
	static COdurState_ReadyExecution* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END
