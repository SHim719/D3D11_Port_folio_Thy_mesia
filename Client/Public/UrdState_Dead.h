#pragma once

#include "UrdState_Base.h"

BEGIN(Client)
class CUrdState_Dead : public CUrdState_Base
{
private:
	CUrdState_Dead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUrdState_Dead() = default;

private:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

public:
	static CUrdState_Dead* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END
