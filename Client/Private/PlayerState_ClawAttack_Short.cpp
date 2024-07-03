#include "PlayerState_ClawAttack_Short.h"


CPlayerState_ClawAttack_Short::CPlayerState_ClawAttack_Short(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_ClawAttack_Short::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_PlagueAttack,
		PlayerState::State_Avoid, PlayerState::State_Parry, PlayerState::State_ClawAttack_Short };

	m_pModel->Bind_Func("ChangeToNextClawAnim", bind(&CPlayerState_ClawAttack_Short::Change_To_NextAttackAnim, this));
		

	return S_OK;
}

void CPlayerState_ClawAttack_Short::OnState_Start(void* pArg)
{
	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_CanRotation(true);
	m_pPlayer->Set_CanNextAttack(false);
	m_pPlayer->Set_Active_DefaultWeapons(false);
	m_pPlayer->Set_Active_Claws(true);
	m_pPlayer->Update_AttackDesc();

	m_pModel->Change_Animation(Corvus_Raven_ClawShortV2_Fast01 + m_iAttackCount++);
}

void CPlayerState_ClawAttack_Short::Update(_float fTimeDelta)
{
	if (m_pPlayer->Can_Rotation() && false == m_pPlayer->Is_LockOn())
	{
		_vector vNewLook = Calc_MoveLook(true);

		if (0.f != vNewLook.m128_f32[0] || 0.f != vNewLook.m128_f32[1])
			Rotate_To_Look(vNewLook, fTimeDelta);
	}

	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);

}

void CPlayerState_ClawAttack_Short::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		_uint iNowPlayingAnim = m_pModel->Get_CurrentAnimIndex();
		if (Corvus_Raven_ClawShortV2_Fast031 == iNowPlayingAnim)
			m_pPlayer->Change_State((_uint)PlayerState::State_ClawAttack_ShortEnd);
		else
			m_pPlayer->Change_State((_uint)PlayerState::State_Idle);

		return;
	}

	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		Check_ExtraStateChange(ePlayerState);

}


void CPlayerState_ClawAttack_Short::OnState_End()
{
	m_iAttackCount = 0;
	m_pPlayer->Set_Active_DefaultWeapons(true);
	m_pPlayer->Set_Active_Claws(false);
	Reset_AttackIdx();

}

void CPlayerState_ClawAttack_Short::Init_AttackDesc()
{
	m_AttackDescs.reserve(7);

	m_AttackDescs.emplace_back(CPlayer::CLAW_R, m_pPlayerStats->Get_PlagueAttackDesc());
	m_AttackDescs.emplace_back(CPlayer::CLAW_L, m_pPlayerStats->Get_PlagueAttackDesc());
	m_AttackDescs.emplace_back(CPlayer::CLAW_R, m_pPlayerStats->Get_PlagueAttackDesc());
	m_AttackDescs.emplace_back(CPlayer::CLAW_R, m_pPlayerStats->Get_PlagueAttackDesc());
	m_AttackDescs.emplace_back(CPlayer::CLAW_L, m_pPlayerStats->Get_PlagueAttackDesc());
	m_AttackDescs.emplace_back(CPlayer::CLAW_R, m_pPlayerStats->Get_PlagueAttackDesc());
	m_AttackDescs.emplace_back(CPlayer::CLAW_R, m_pPlayerStats->Get_PlagueAttackDesc());
}

void CPlayerState_ClawAttack_Short::Check_ExtraStateChange(PlayerState eState)
{
	if (PlayerState::State_ClawAttack_Short == eState)
	{
		if (true == m_pPlayer->Can_NextAttack() && m_iAttackCount < m_iMaxAttackCount)
			OnState_Start(nullptr);
	}
	else
	{
		m_pPlayer->Change_State((_uint)eState);
	}
}

void CPlayerState_ClawAttack_Short::Change_To_NextAttackAnim()
{
	ADD_EVENT(bind(&CModel::Change_AnimationWithStartFrame, m_pModel, Corvus_Raven_ClawShortV2_Fast031, 18, 0.1f, true));
}

CPlayerState_ClawAttack_Short* CPlayerState_ClawAttack_Short::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_ClawAttack_Short* pInstance = new CPlayerState_ClawAttack_Short(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_ClawAttack_Short"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ClawAttack_Short::Free()
{
	__super::Free();
}
