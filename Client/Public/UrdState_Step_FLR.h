#pragma once

#include "UrdState_Base.h"

BEGIN(Client)
class CUrdState_Step_FLR : public CUrdState_Base
{
private:
	CUrdState_Step_FLR(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUrdState_Step_FLR() = default;

private:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

private:
	void Decide_State()					override;
	void Decide_StepAnim();

public:
	static CUrdState_Step_FLR* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END