#pragma once

#include "VillagerFState_Base.h"

BEGIN(Client)
class CVillagerFState_Stunned_Start final : public CVillagerFState_Base
{
private:
	CVillagerFState_Stunned_Start(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CVillagerFState_Stunned_Start() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

private:
	void OnHit(const ATTACKDESC& AttackDesc) override;


public:
	static CVillagerFState_Stunned_Start* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END


