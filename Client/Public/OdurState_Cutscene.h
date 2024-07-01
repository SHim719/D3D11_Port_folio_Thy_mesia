#pragma once

#include "OdurState_Base.h"

BEGIN(Client)
class COdurState_Cutscene : public COdurState_Base
{
protected:
	COdurState_Cutscene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COdurState_Cutscene() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void OnState_End()					override;


private:
	void FadeOut_Cutscene();

private:
	CGameObject*	m_pCard = { nullptr };

public:
	static COdurState_Cutscene* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END

