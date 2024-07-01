#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_ChargeStart final : public CPlayerState_Base
{
private:
	CPlayerState_ChargeStart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_ChargeStart() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

	void Check_ExtraStateChange(PlayerState eState)		override;

private:
	_bool m_bCanClawLong = { false };

public:
	void Set_CanClawLongAttack(_bool b) {
		m_bCanClawLong = b;
	}

public:
	static CPlayerState_ChargeStart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

