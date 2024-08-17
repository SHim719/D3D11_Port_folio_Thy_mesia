#pragma once

#include "UrdState_Base.h"

BEGIN(Client)
class CUrdState_Parry : public CUrdState_Base
{
private:
	CUrdState_Parry(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUrdState_Parry() = default;

private:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDetla)	override;
	void OnState_End()					override;

private:
	void Decide_State()					override;
	void Check_ChangeSkill3();

public:
	static CUrdState_Parry* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END
