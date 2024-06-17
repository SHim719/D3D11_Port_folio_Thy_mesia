#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_Parried final : public CPlayerState_Base
{
private:
	CPlayerState_Parried(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_Parried() = default;

public:
	HRESULT Initialize(void* pArg)	override;
	void OnState_Start(void* pArg)	override;
	void OnGoing(_float fTimeDelta)	override;
	void OnState_End()				override;

public:
	static CPlayerState_Parried* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

