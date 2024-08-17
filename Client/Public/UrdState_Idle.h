#pragma once

#include "UrdState_Base.h"

BEGIN(Client)
class CUrdState_Idle : public CUrdState_Base
{
protected:
	CUrdState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUrdState_Idle() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void OnState_End()					override;

	void OnHit(const ATTACKDESC& AttackDesc)				override;

public:
	static CUrdState_Idle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END