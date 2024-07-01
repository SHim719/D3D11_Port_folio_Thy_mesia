#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_Avoid final : public CPlayerState_Base
{
private:
	CPlayerState_Avoid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_Avoid() = default;

private:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

	void OnHit(const ATTACKDESC& AttackDesc)		override;
	void Check_ExtraStateChange(PlayerState eState)	override;

private:
	_bool m_bOneMore = { false };

public:
	void Enable_Avoid() { 
		m_bOneMore = true; }

private:
	void Decide_Animation();

public:
	static CPlayerState_Avoid* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

