#include "PlayerState_ClawAttack_Long2.h"

CPlayerState_ClawAttack_Long2::CPlayerState_ClawAttack_Long2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_ClawAttack_Long2::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_PlagueAttack,
		PlayerState::State_Avoid, PlayerState::State_Parry, PlayerState::State_ClawAttack_Long };

	return S_OK;
}

void CPlayerState_ClawAttack_Long2::OnState_Start(void* pArg)
{
	Reset_AttackIdx();
	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_CanRotation(false);
	m_pPlayer->Set_Active_DefaultWeapons(false);
	m_pPlayer->Set_Active_Claws(true);

	m_pPlayer->Update_AttackDesc();

	m_pModel->Change_AnimationWithStartFrame(Corvus_Raven_ClawLong_L01, 27, 0.1f);

	RADIALBLUR_DESCS RadialDescs{};
	RadialDescs.fBlurRadius = 10.f;
	RadialDescs.fBlurStrength = 1.5f;

	m_pGameInstance->Active_RadialBlur(RadialDescs);
	m_pGameInstance->Update_BlurCenterWorld(m_pPlayer->Get_Center());
}

void CPlayerState_ClawAttack_Long2::Update(_float fTimeDelta)
{
	if (m_pPlayer->Can_Rotation() && false == m_pPlayer->Is_LockOn())
	{
		_vector vNewLook = Calc_MoveLook(true);

		if (0.f != vNewLook.m128_f32[0] || 0.f != vNewLook.m128_f32[1])
			Rotate_To_Look(vNewLook, fTimeDelta);
	}

	if (!m_pPlayer->Is_CollEnemy())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);

}

void CPlayerState_ClawAttack_Long2::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
		return;
	}

	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		Check_ExtraStateChange(ePlayerState);

	//if (!= ePlayerState)

}

void CPlayerState_ClawAttack_Long2::OnState_End()
{
	m_pPlayer->Set_Active_DefaultWeapons(true);
	m_pPlayer->Set_Active_Claws(false);

	m_pGameInstance->Inactive_RadialBlur(0.5f);
}

void CPlayerState_ClawAttack_Long2::Init_AttackDesc()
{
	m_AttackDescs.reserve(1);

	m_AttackDescs.emplace_back(CPlayer::CLAW_R, m_pPlayerStats->Get_PlagueAttackDesc());
}


CPlayerState_ClawAttack_Long2* CPlayerState_ClawAttack_Long2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_ClawAttack_Long2* pInstance = new CPlayerState_ClawAttack_Long2(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_ClawAttack_Long2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ClawAttack_Long2::Free()
{
	__super::Free();
}
