#pragma once

#include "JokerState_Base.h"

BEGIN(Client)
class CJokerState_Walk : public CJokerState_Base
{
protected:
	CJokerState_Walk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CJokerState_Walk() = default;

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
	void Move(_float fTimeDelta);

public:
	static CJokerState_Walk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END


