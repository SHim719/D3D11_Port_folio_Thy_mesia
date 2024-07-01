#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_ClawAttack_ShortEnd final : public CPlayerState_Base
{
private:
	CPlayerState_ClawAttack_ShortEnd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_ClawAttack_ShortEnd() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

public:
	static CPlayerState_ClawAttack_ShortEnd* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

