#pragma once

#include "UrdState_Base.h"

BEGIN(Client)
class CUrdState_Hit : public CUrdState_Base
{
protected:
	CUrdState_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUrdState_Hit() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

	void OnHit(const ATTACKDESC& AttackDesc)				override;

private:
	void Decide_State()		override;


private:
	_int	m_iHitCount = { 0 };
	_int	m_iMaxHitCount = { 10 };


public:
	static CUrdState_Hit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END