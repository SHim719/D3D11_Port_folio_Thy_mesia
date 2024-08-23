#pragma once
#include "PlayerState_Base.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)
class CPlayerState_Executed final : public CPlayerState_Base
{
private:
	CPlayerState_Executed(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_Executed() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void OnState_End()					override;

	void OnHit(const ATTACKDESC& AttackDesc)	override;
private:
	EXECUTEDDESC m_ExecutedDesc;

private:
	void Change_StateToGetUp();

public:
	static CPlayerState_Executed* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

