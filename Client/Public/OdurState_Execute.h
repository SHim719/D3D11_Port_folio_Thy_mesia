#pragma once

#include "OdurState_Base.h"

BEGIN(Client)
class COdurState_Execute final : public COdurState_Base
{
private:
	COdurState_Execute(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COdurState_Execute() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

public:
	static COdurState_Execute* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END
