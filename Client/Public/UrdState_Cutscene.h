#pragma once

#include "UrdState_Base.h"

BEGIN(Client)
class CUrdState_Cutscene : public CUrdState_Base
{
protected:
	CUrdState_Cutscene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUrdState_Cutscene() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void OnState_End()					override;

private:
	void FadeOut_Cutscene();

public:
	static CUrdState_Cutscene* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END

