#pragma once

#include "VillagerMState_Base.h"

BEGIN(Client)
class CVillagerMState_Attack : public CVillagerMState_Base
{
protected:
	CVillagerMState_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CVillagerMState_Attack() = default;

private:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

	void Init_AttackDesc()				override;

private:
	void Decide_Attack();
public:
	static CVillagerMState_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END




