#pragma once

#include "UrdState_Base.h"

BEGIN(Client)
class CUrdState_ExtraAttack : public CUrdState_Base
{
private:
	CUrdState_ExtraAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUrdState_ExtraAttack() = default;

private:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

	void Init_AttackDesc()				override;

public:
	static CUrdState_ExtraAttack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END