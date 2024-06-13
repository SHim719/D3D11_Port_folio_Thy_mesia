#pragma once
#include "State_Base.h"


BEGIN(Client)
class COdurState_Base : public CState_Base
{
protected:
	COdurState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COdurState_Base() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start()				override;
	void OnGoing(_float fTimeDelta)		override;
	void OnState_End()					override;

protected:


public:
	void Free() override;
};

END

