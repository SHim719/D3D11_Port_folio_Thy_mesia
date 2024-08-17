#pragma once

#include "OdurState_Base.h"

BEGIN(Client)
class COdurState_Parry : public COdurState_Base
{
protected:
	COdurState_Parry(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COdurState_Parry() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDetla)	override;
	void OnState_End()					override;

private:
	vector<pair<_uint, ATTACKDESC>> m_ParryAttackDescs[2];

private:
	void Init_AttackDesc()				override;

	void Decide_Animation();

public:
	static COdurState_Parry* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END
