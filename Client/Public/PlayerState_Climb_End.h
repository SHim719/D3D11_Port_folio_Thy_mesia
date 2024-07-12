#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_Climb_End final : public CPlayerState_Base
{
private:
	CPlayerState_Climb_End(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_Climb_End() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

private:
	_bool m_bIsUp = { false };
	_float m_fMoveRate = { 1.f };

public:
	static CPlayerState_Climb_End* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

