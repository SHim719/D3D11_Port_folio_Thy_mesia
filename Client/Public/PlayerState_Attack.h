#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_Attack final : public CPlayerState_Base
{
private:
	CPlayerState_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_Attack() = default;

private:
	HRESULT Initialize(void* pArg)					override;
	void OnState_Start(void* pArg)					override;
	void Update(_float fTimeDelta)					override;
	void Late_Update(_float fTimeDelta)				override;
	void OnState_End()								override;

	void Init_AttackDesc()							override;
	void Check_ExtraStateChange(PlayerState eState)	override;

	void Decide_ExecutionState(class CEnemy* pExecutionEnemy);

	void Check_NextAttack();

	void Play_AttackSound();
private:
	_uint			m_iNowComboCnt = 0;
	_uint			m_iMaxComboCnt = 5;

public:
	static CPlayerState_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

