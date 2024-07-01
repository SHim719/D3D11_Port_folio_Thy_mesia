#pragma once

#include "OdurState_Base.h"

BEGIN(Client)
class COdurState_Walk : public COdurState_Base
{
protected:
	COdurState_Walk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COdurState_Walk() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void OnState_End()					override;

	void OnHit(const ATTACKDESC& AttackDesc)				override;
	
private:
	_float	m_fWalkSpeed = { 2.f };
	_int	m_iDir = { 0 };

private:
	void Decide_Animation();

public:
	static COdurState_Walk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END