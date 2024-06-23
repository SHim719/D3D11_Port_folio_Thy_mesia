#pragma once


#include "OdurState_Base.h"

BEGIN(Client)
class COdurState_KickCombo : public COdurState_Base
{
protected:
	COdurState_KickCombo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COdurState_KickCombo() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void OnGoing(_float fTimeDelta)		override;
	void OnState_End()					override;

private:
	void Init_AttackDesc()				override;
	void Change_To_Disappear();

public:
	static COdurState_KickCombo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END


