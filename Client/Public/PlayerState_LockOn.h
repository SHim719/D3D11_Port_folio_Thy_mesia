#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_LockOn final : public CPlayerState_Base
{
private:
	CPlayerState_LockOn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_LockOn() = default;

public:
	HRESULT Initialize(void* pArg)			override;
	void OnState_Start(void* pArg)			override;
	void Update(_float fTimeDelta)			override;
	void Late_Update(_float fTimeDelta)		override;
	void OnState_End()						override;

private:
	void Change_WalkAnimation();
		
private:
	_float2 m_vPrevMoveAxis = { 0.f, 0.f };

	_bool m_bAxisChanging = { false };
	_float m_fChangingTerm = { 0.05f };
	_float m_fChangingTermAcc = { 0.f };

public:
	static CPlayerState_LockOn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

