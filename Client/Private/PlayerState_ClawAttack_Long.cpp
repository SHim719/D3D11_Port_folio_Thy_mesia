#include "PlayerState_ClawAttack_Long.h"

CPlayerState_ClawAttack_Long::CPlayerState_ClawAttack_Long(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_ClawAttack_Long::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_PlagueAttack,
		PlayerState::State_Avoid, PlayerState::State_Parry };

	return S_OK;
}

void CPlayerState_ClawAttack_Long::OnState_Start(void* pArg)
{
	Reset_AttackIdx();
	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_CanRotation(true);
	m_pPlayer->Set_Active_DefaultWeapons(false);
	m_pPlayer->Set_Active_Claws(true);

	m_pPlayer->Update_AttackDesc();

	m_pModel->Change_AnimationWithStartFrame(Corvus_Raven_ClawLong_L01, 27, 0.05f);
}

void CPlayerState_ClawAttack_Long::Update(_float fTimeDelta)
{

	if (m_pPlayer->Can_Rotation() && false == m_pPlayer->Is_LockOn())
	{
		_vector vNewLook = Calc_MoveLook(true);

		if (0.f != vNewLook.m128_f32[0] || 0.f != vNewLook.m128_f32[1])
			Rotate_To_Look(vNewLook, fTimeDelta);
	}

	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);

}

void CPlayerState_ClawAttack_Long::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
		return;
	}
		
	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		Check_ExtraStateChange(ePlayerState);
}


void CPlayerState_ClawAttack_Long::OnState_End()
{
	m_pPlayer->Set_Active_DefaultWeapons(true);
	m_pPlayer->Set_Active_Claws(false);
}

void CPlayerState_ClawAttack_Long::Init_AttackDesc()
{
	m_AttackDescs.reserve(1);

	m_AttackDescs.emplace_back(CPlayer::CLAW_R, m_pPlayerStats->Get_PlagueAttackDesc());                                               
}


CPlayerState_ClawAttack_Long* CPlayerState_ClawAttack_Long::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_ClawAttack_Long* pInstance = new CPlayerState_ClawAttack_Long(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_ClawAttack_Long"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ClawAttack_Long::Free()
{
	__super::Free();
}
