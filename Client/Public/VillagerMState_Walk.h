#pragma once

#include "VillagerMState_Base.h"

BEGIN(Client)
class CVillagerMState_Walk : public CVillagerMState_Base
{
protected:
	CVillagerMState_Walk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CVillagerMState_Walk() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

private:
	_float	m_fTimeAcc = { 0.f };
	_float	m_fWalkTime = { 0.f };
	_float  m_fWalkSpeed = { 2.f };

	_int	m_iDir = { 0 };

private:
	void Decide_WalkAnimation();
	void Decide_State()					override;
	void Move(_float fTimeDelta);

public:
	static CVillagerMState_Walk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END


