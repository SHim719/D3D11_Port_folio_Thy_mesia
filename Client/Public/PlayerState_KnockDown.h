#pragma once

#include "PlayerState_Base.h"


BEGIN(Client)

class CPlayerState_KnockDown final : public CPlayerState_Base
{
private:
	CPlayerState_KnockDown(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_KnockDown() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

private:
	_bool		m_bCalcDuration = { false };
	_float		m_fDuration = { 2.f };
	_float		m_fTimeAcc = { 0.f };

public:
	static CPlayerState_KnockDown* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END

