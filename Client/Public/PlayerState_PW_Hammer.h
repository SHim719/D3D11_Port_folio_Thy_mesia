#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_PW_Hammer final : public CPlayerState_Base
{
private:
	CPlayerState_PW_Hammer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_PW_Hammer() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

	void Init_AttackDesc()				override;

public:
	static CPlayerState_PW_Hammer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

