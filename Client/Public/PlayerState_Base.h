#pragma once

#include "State_Base.h"
#include "Player.h"
#include "PlayerStats.h"
#include "Main_Camera.h"

BEGIN(Client)
class CPlayerState_Base : public CState_Base
{
protected:
	CPlayerState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_Base() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void OnGoing(_float fTimeDelta)		override;
	void OnState_End()					override;

	void OnHit(void* pArg)				override;

protected:
	_vector Calc_MoveLook(_bool IsCamOriented);
	void Rotate_To_Look(_vector vNewLook, _float fTimeDelta);

	_bool Check_StateChange(PlayerState eState);
	PlayerState Decide_State();

protected:
	CPlayer*			m_pPlayer = { nullptr };
	CMain_Camera*		m_pMain_Camera = { nullptr };
	CPlayerStats*		m_pPlayerStats = { nullptr };
	vector<PlayerState> m_PossibleStates;

	_float2				m_vMoveAxis = { 0.f, 0.f };

	_float				m_fRotRate = { 10.f };
	_float				m_fJogSpeed = { 4.f };
	_float				m_fSprintSpeed = { 6.f };
public:
	void Free() override;
};

END

