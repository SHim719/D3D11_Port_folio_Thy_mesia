#pragma once

#include "JokerState_Base.h"

BEGIN(Client)
class CJokerState_Executed_Start final : public CJokerState_Base
{
private:
	CJokerState_Executed_Start(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CJokerState_Executed_Start() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

private:
	ATTACHDESC m_AttachDesc = {};

public:
	static CJokerState_Executed_Start* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END


