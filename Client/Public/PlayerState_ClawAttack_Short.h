#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_ClawAttack_Short final : public CPlayerState_Base
{
private:
	CPlayerState_ClawAttack_Short(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_ClawAttack_Short() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

	void Init_AttackDesc()							override;
	void Check_ExtraStateChange(PlayerState eState)	override;

private:
	void Change_To_NextAttackAnim();

private:
	_uint m_iAttackCount = { 0 };
	_uint m_iMaxAttackCount = { 3 };

public:
	static CPlayerState_ClawAttack_Short* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

