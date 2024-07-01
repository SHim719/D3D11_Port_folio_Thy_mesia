#pragma once

#include "OdurState_Base.h"

BEGIN(Client)
class COdurState_DisappearWalk : public COdurState_Base
{
protected:
	COdurState_DisappearWalk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COdurState_DisappearWalk() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void OnState_End()					override;

private:
	_int m_iDir = { 0 };


private:
	void Decide_Animation();

public:
	static COdurState_DisappearWalk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END
