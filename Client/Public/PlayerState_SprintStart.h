#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_SprintStart final : public CPlayerState_Base
{
private:
	CPlayerState_SprintStart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_SprintStart() = default;

public:
	HRESULT Initialize(void* pArg)	override;
	void OnState_Start()			override;
	void OnGoing(_float fTimeDelta)	override;
	void OnState_End()				override;


public:
	static CPlayerState_SprintStart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

