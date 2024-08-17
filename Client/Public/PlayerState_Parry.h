#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_Parry final : public CPlayerState_Base
{
private:
	CPlayerState_Parry(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_Parry() = default;

public:
	HRESULT Initialize(void* pArg)			override;
	void OnState_Start(void* pArg)			override;
	void Update(_float fTimeDelta)			override;
	void Late_Update(_float fTimeDelta)		override;
	void OnState_End()						override;

	void OnHit(const ATTACKDESC& AttackDesc)			override;

private:
	_int  m_iParryDir = 0;


public:
	static CPlayerState_Parry* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

