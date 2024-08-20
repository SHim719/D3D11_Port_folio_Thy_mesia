#include "PlayerState_ChargeStart.h"

CPlayerState_ChargeStart::CPlayerState_ChargeStart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_ChargeStart::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_PlagueAttack,
		PlayerState::State_Avoid, PlayerState::State_Parry, PlayerState::State_ClawAttack_Long };

	m_pModel->Bind_Func("Enable_ClawAttackLong", bind(&CPlayerState_ChargeStart::Set_CanClawLongAttack, this, true));

	return S_OK;
}

void CPlayerState_ChargeStart::OnState_Start(void* pArg)
{
	m_pPlayer->Set_CanNextState(true);
	m_pPlayer->Set_CanRotation(true);
	m_pPlayer->Set_Active_DefaultWeapons(false);
	m_pPlayer->Set_Active_Claws(true);
	m_bCanClawLong = false;

	PLAY_SOUND(L"Claw_Start", false, 1.f);

	m_pModel->Change_Animation(Corvus_Raven_ClawCommonV2_ChargeStart);
}

void CPlayerState_ChargeStart::Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_ChargeComplete);
	
	if (m_pPlayer->Can_Rotation() && false == m_pPlayer->Is_LockOn())
	{
		_vector vNewLook = Calc_MoveLook(true);

		if (0.f != vNewLook.m128_f32[0] || 0.f != vNewLook.m128_f32[1])
			Rotate_To_Look(vNewLook, fTimeDelta);
	}

	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CPlayerState_ChargeStart::Late_Update(_float fTimeDelta)
{
	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		Check_ExtraStateChange(ePlayerState);

}


void CPlayerState_ChargeStart::OnState_End()
{
	m_pPlayer->Set_Active_DefaultWeapons(true);
	m_pPlayer->Set_Active_Claws(false);
}

void CPlayerState_ChargeStart::Check_ExtraStateChange(PlayerState eState)
{
	switch (eState)
	{
	case PlayerState::State_ClawAttack_Long:
	{
		if (m_pPlayerStats->Is_ShortClaw())
			m_pPlayer->Change_State((_uint)PlayerState::State_ClawAttack_Short);
		else if (m_bCanClawLong)
			m_pPlayer->Change_State((_uint)PlayerState::State_ClawAttack_Long);
		break;
	}

	case PlayerState::State_PlagueAttack:
	{
		Check_PlagueAttack();
		break;
	}

	default:
	{
		m_pPlayer->Change_State((_uint)eState);
		break;
	}
	}
}


CPlayerState_ChargeStart* CPlayerState_ChargeStart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_ChargeStart* pInstance = new CPlayerState_ChargeStart(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_ChargeStart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ChargeStart::Free()
{
	__super::Free();
}
