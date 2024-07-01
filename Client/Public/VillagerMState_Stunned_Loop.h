#pragma once

#include "VillagerMState_Base.h"

BEGIN(Client)
class CVillagerMState_Stunned_Loop final : public CVillagerMState_Base
{
private:
	CVillagerMState_Stunned_Loop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CVillagerMState_Stunned_Loop() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

private:
	void OnHit(const ATTACKDESC& AttackDesc) override;


public:
	static CVillagerMState_Stunned_Loop* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END


