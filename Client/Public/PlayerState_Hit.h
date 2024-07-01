#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_Hit final : public CPlayerState_Base
{
private:
	CPlayerState_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_Hit() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

private:
	_int  m_iHitDir = 0;

public:
	static CPlayerState_Hit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

