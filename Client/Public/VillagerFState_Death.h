#pragma once

#include "VillagerFState_Base.h"

BEGIN(Client)
class CVillagerFState_Death final : public CVillagerFState_Base
{
private:
	CVillagerFState_Death(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CVillagerFState_Death() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

	void OnHit(const ATTACKDESC& AttackDesc) override;
public:
	static CVillagerFState_Death* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END




