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
	void OnState_Start()			override;
	void OnGoing(_float fTimeDelta)	override;
	void OnState_End()				override;

private:
	void Enable_NextAttack();

private:
	_bool			m_bCanNextAttack = false;
	_uint			m_iMaxComboCnt = 5;
	_uint			m_iNowComboCnt = 0;
public:
	static CPlayerState_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

