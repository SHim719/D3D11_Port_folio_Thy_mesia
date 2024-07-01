#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_Steal final : public CPlayerState_Base
{
private:
	CPlayerState_Steal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_Steal() = default;

public:
	HRESULT Initialize(void* pArg)	override;
	void OnState_Start(void* pArg)	override;
	void Update(_float fTimeDelta)	override;
	void OnState_End()				override;

public:
	static CPlayerState_Steal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

