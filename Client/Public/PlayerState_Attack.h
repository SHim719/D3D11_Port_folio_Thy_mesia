#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_Attack final : public CPlayerState_Base
{
private:
	CPlayerState_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_Attack() = default;

private:
	HRESULT Initialize(void* pArg)	override;
	void OnState_Start(void* pArg)	override;
	void OnGoing(_float fTimeDelta)	override;
	void OnState_End()				override;

	void Init_AttackDesc()			override;

private:
	_bool			m_bCanNextAttack = { false };
	_uint			m_iNowComboCnt = 0;

private:
	void Set_CanNextAttack(_bool bCanNextAttack) {
		m_bCanNextAttack = bCanNextAttack;
	}

public:
	static CPlayerState_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

