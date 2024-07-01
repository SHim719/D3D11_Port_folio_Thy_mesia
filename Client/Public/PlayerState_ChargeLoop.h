#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_ChargeLoop final : public CPlayerState_Base
{
private:
	CPlayerState_ChargeLoop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_ChargeLoop() = default;

public:
	HRESULT Initialize(void* pArg)	override;
	void OnState_Start(void* pArg)	override;
	void Update(_float fTimeDelta)	override;
	void Late_Update(_float fTimeDelta) override;
	void OnState_End()				override;

public:
	static CPlayerState_ChargeLoop* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

