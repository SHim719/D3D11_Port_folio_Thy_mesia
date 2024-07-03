#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_Plunder final : public CPlayerState_Base
{
private:
	CPlayerState_Plunder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_Plunder() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

	void Init_AttackDesc()				override;

private:
	class CEnemy* m_pTargetEnemy = { nullptr };

private:
	void Plunder_PlagueWeapon();
	void Plunder_Attack();

public:
	static CPlayerState_Plunder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

