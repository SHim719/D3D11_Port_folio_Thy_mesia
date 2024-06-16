#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_ParrySuccess final : public CPlayerState_Base
{
private:
	CPlayerState_ParrySuccess(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_ParrySuccess() = default;

public:
	HRESULT Initialize(void* pArg)	override;
	void OnState_Start(void* pArg)	override;
	void OnGoing(_float fTimeDelta)	override;
	void OnState_End()				override;

	void OnHit(void* pArg)			override;

public:
	static CPlayerState_ParrySuccess* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

