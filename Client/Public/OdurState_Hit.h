#pragma once

#include "OdurState_Base.h"

BEGIN(Client)
class COdurState_Hit : public COdurState_Base
{
protected:
	COdurState_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COdurState_Hit() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

	void OnHit(const ATTACKDESC& AttackDesc)				override;

private:
	_int	m_iHitCount = { 0 };
	_int	m_iMaxHitCount = { 15 };

public:
	static COdurState_Hit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END