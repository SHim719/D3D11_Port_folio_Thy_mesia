#pragma once

#include "VillagerFState_Base.h"

BEGIN(Client)
class CVillagerFState_Attack1 : public CVillagerFState_Base
{
protected:
	CVillagerFState_Attack1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CVillagerFState_Attack1() = default;

private:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

	void Init_AttackDesc()				override;

public:
	static CVillagerFState_Attack1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END

