#pragma once

#include "JokerState_Base.h"

BEGIN(Client)
class CJokerState_TurnAttack_R final : public CJokerState_Base
{
private:
	CJokerState_TurnAttack_R(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CJokerState_TurnAttack_R() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

	void Init_AttackDesc()				override;

public:
	static CJokerState_TurnAttack_R* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END

