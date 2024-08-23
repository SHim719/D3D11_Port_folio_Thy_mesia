#pragma once

#include "UrdState_Base.h"

BEGIN(Client)
class CUrdState_Stunned_Start final : public CUrdState_Base
{
private:
	CUrdState_Stunned_Start(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUrdState_Stunned_Start() = default;

private:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

	void OnHit(const ATTACKDESC& AttackDesc)	override;

public:
	static CUrdState_Stunned_Start* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END
