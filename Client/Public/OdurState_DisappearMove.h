#pragma once

#include "OdurState_Base.h"

BEGIN(Client)
class COdurState_DisappearMove : public COdurState_Base
{
protected:
	COdurState_DisappearMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COdurState_DisappearMove() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void OnState_End()					override;

private:
	_int m_iDir = { 0 };

private:
	void Decide_Animation();

	void ChangeState_Appear();

public:
	static COdurState_DisappearMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END
