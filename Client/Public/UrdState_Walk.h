#pragma once

#include "UrdState_Base.h"

BEGIN(Client)
class CUrdState_Walk : public CUrdState_Base
{
protected:
	CUrdState_Walk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUrdState_Walk() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

private:
	_float	m_fWalkSpeed = { 2.f };
	_int	m_iDir = { 0 };

private:
	void Decide_Animation();

public:
	static CUrdState_Walk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END