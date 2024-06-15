#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_Attack final : public CPlayerState_Base
{
private:
	CPlayerState_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_Attack() = default;

public:
	HRESULT Initialize(void* pArg)	override;
	void OnState_Start(void* pArg)	override;
	void OnGoing(_float fTimeDelta)	override;
	void OnState_End()				override;

private:
	_bool			m_bNextAttack = { false };
	_uint			m_iNowComboCnt = 0;

private:
	void Enable_NextAttack() {
		m_bNextAttack = true;
	}

	void Disable_NextAttack() {
		m_bNextAttack = false;
	}

public:
	static CPlayerState_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

