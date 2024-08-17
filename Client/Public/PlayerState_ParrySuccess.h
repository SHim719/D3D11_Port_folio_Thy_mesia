#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_ParrySuccess final : public CPlayerState_Base
{
public:
	typedef struct tagParryDesc
	{
		_int iParryDir = { 0 };
		ATTACKDESC AttackDesc{};
	}PARRY_DESC;

private:
	CPlayerState_ParrySuccess(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_ParrySuccess() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta) override;
	void OnState_End()					override;

private:
	void OnHit(const ATTACKDESC& AttackDesc)	override;

	void Play_CameraShake(const ATTACKDESC& AttackDesc);
	void Set_PushStrength(_float fStrength) {
		m_fPushStrength = fStrength;
	}

private:
	_int	m_iParryDir = { 0 };
	_float	m_fPushStrength = { 1.f };

public:
	static CPlayerState_ParrySuccess* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

