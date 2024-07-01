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
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void OnState_End()					override;

	void OnHit(const ATTACKDESC& AttackDesc)				override;


public:
	static COdurState_Idle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END