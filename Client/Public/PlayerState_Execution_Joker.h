#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_Execution_Joker final : public CPlayerState_Base
{
private:
	CPlayerState_Execution_Joker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_Execution_Joker() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

private:
	class CEnemy*	m_pExecutionEnemy = { nullptr };


public:
	static CPlayerState_Execution_Joker* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

