#pragma once

#include "OdurState_Base.h"

BEGIN(Client)
class COdurState_Idle : public COdurState_Base
{
protected:
	COdurState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COdurState_Idle() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start()				override;
	void OnGoing(_float fTimeDelta)		override;
	void OnState_End()					override;

protected:


public:
	static COdurState_Idle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END