#pragma once

#include "PlayerState_Base.h"


BEGIN(Client)

class CPlayerState_Idle final : public CPlayerState_Base
{
private:
	CPlayerState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_Idle() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

public:
	static CPlayerState_Idle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END

