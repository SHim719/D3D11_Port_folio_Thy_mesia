#pragma once

#include "VillagerFState_Base.h"

BEGIN(Client)
class CVillagerFState_Hit final : public CVillagerFState_Base
{
private:
	CVillagerFState_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CVillagerFState_Hit() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

private:
	void OnHit(const ATTACKDESC& AttackDesc) override;

private:
	_int	m_iHitCount = { 0 };
	_int	m_iMaxHitCount = { 15 };


public:
	static CVillagerFState_Hit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END

