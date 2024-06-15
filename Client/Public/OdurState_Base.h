#pragma once

#include "State_Base.h"
#include "Odur.h"

BEGIN(Client)
class COdurState_Base : public CState_Base
{
protected:
	COdurState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COdurState_Base() = default;

public:
	HRESULT Initialize(void* pArg)					override;
	void OnState_Start(void* pArg)					override;
	void OnGoing(_float fTimeDelta)					override;
	void OnState_End()								override;

protected:
	COdur* m_pOdur = { nullptr };

public:
	void Free() override;
};

END

