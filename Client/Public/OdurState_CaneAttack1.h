#pragma once

#include "OdurState_Base.h"

BEGIN(Client)
class COdurState_CaneAttack1 : public COdurState_Base
{
protected:
	COdurState_CaneAttack1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COdurState_CaneAttack1() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void OnGoing(_float fTimeDelta)		override;
	void OnState_End()					override;

public:
	static COdurState_CaneAttack1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END