#include "PlayerState_ChargeLoop.h"

CPlayerState_ChargeLoop::CPlayerState_ChargeLoop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_ChargeLoop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_StealRush, PlayerState::State_Avoid, PlayerState::State_Parry };

	return S_OK;
}

void CPlayerState_ChargeLoop::OnState_Start(void* pArg)
{
	m_pPlayer->Set_CanNextState(true);
	m_pPlayer->Set_CanRotation(true);
	m_pPlayer->Set_Active_Claws(true);
	m_pPlayer->Set_Active_DefaultWeapons(false);

	m_pModel->Change_Animation(Corvus_Raven_ClawCommon_ChargeLoop);
}

void CPlayerState_ChargeLoop::Update(_float fTimeDelta)
{
	if (false == m_pPlayer->Is_LockOn())
	{
		_vector vNewLook = Calc_MoveLook(true);

		if (0.f != vNewLook.m128_f32[0] || 0.f != vNewLook.m128_f32[1])
			Rotate_To_Look(vNewLook, fTimeDelta);
	}
}

void CPlayerState_ChargeLoop::Late_Update(_float fTimeDelta)
{
	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		m_pPlayer->Change_State((_uint)ePlayerState);
}


void CPlayerState_ChargeLoop::OnState_End()
{
	m_pPlayer->Set_Active_DefaultWeapons(true);
	m_pPlayer->Set_Active_Claws(false);
}


CPlayerState_ChargeLoop* CPlayerState_ChargeLoop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_ChargeLoop* pInstance = new CPlayerState_ChargeLoop(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_ChargeLoop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ChargeLoop::Free()
{
	__super::Free();
}