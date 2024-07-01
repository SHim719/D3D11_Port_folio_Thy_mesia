#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayerState_Execution_Default final : public CPlayerState_Base
{
private:
	CPlayerState_Execution_Default(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_Execution_Default() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

	void Check_ExtraStateChange(PlayerState eState)	override;
private:
	_float			m_fRushSpeed = { 25.f };
	_int			m_iExecutionCount = 0;
	class CEnemy*	m_pExecutionEnemy = { nullptr };
	_bool			m_bNoRootMove = { true };

	_float4			m_vStartPos = {};
private:
	void Set_NoRootMove(_bool bRootMove) {
		m_bNoRootMove = bRootMove;
	}

public:
	static CPlayerState_Execution_Default* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};
END

