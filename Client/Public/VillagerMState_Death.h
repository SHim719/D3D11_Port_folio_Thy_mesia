#pragma once

#include "VillagerMState_Base.h"

BEGIN(Client)
class CVillagerMState_Death final : public CVillagerMState_Base
{
private:
	CVillagerMState_Death(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CVillagerMState_Death() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

	void OnHit(const ATTACKDESC& AttackDesc) override;
public:
	static CVillagerMState_Death* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END




