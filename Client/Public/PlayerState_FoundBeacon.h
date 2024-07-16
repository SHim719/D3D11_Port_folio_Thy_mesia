#pragma once

#include "PlayerState_Base.h"

BEGIN(Client)

class CPlayerState_FoundBeacon final : public CPlayerState_Base
{
private:
	CPlayerState_FoundBeacon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_FoundBeacon() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

private:
	void Active_Chair();

private:
	class CArchive_Chair* m_pActivating_Chair = { nullptr };

public:
	static CPlayerState_FoundBeacon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END

