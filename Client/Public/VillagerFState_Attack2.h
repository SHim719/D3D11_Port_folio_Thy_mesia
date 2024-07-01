#pragma once

#include "VillagerFState_Base.h"

BEGIN(Client)
class CVillagerFState_Attack2 final : public CVillagerFState_Base
{
private:
	CVillagerFState_Attack2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CVillagerFState_Attack2() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

	void Init_AttackDesc()				override;

private:
	void Change_To_NextAttackAnim();

public:
	static CVillagerFState_Attack2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END

