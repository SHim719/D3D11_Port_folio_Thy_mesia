#pragma once

#include "OdurState_Base.h"

BEGIN(Client)
class COdurState_Appear : public COdurState_Base
{
protected:
	COdurState_Appear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COdurState_Appear() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void OnGoing(_float fTimeDelta)		override;
	void OnState_End()					override;

private:
	void Decide_Animation(void* pArg);

public:
	static COdurState_Appear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END
