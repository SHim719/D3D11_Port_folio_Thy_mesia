#include "PlayerState_Plunder.h"

#include "Enemy.h"

CPlayerState_Plunder::CPlayerState_Plunder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Plunder::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_ChargeStart, PlayerState::State_Avoid, PlayerState::State_Parry };

	m_pModel->Bind_Func("Plunder_PlagueWeapon", bind(&CPlayerState_Plunder::Plunder_PlagueWeapon, this));
	m_pModel->Bind_Func("Plunder_Attack", bind(&CPlayerState_Plunder::Plunder_Attack, this));

	return S_OK;
}

void CPlayerState_Plunder::OnState_Start(void* pArg)
{
	Reset_AttackIdx();

	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_Stanced(true);
	m_pPlayer->Set_Active_DefaultWeapons(false);
	m_pPlayer->Inactive_AllWeaponColliders();

	m_pTargetEnemy = (CEnemy*)pArg;
	m_pPlayer->Update_AttackDesc();

	PLAY_SOUND(L"Plunder_Hit_Success", false, 0.7f);

	m_pModel->Change_Animation(Corvus_Raven_ClawShort_PlunderAttack2TT);
}

void CPlayerState_Plunder::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CPlayerState_Plunder::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);

	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		Check_ExtraStateChange(ePlayerState);

}

void CPlayerState_Plunder::OnState_End()
{
	m_pPlayer->Set_Stanced(false);
	m_pPlayer->Set_Active_DefaultWeapons(true);
	m_pPlayer->Set_Active_Claws(false);
}

void CPlayerState_Plunder::Init_AttackDesc()
{
	m_AttackDescs.reserve(1);

	ATTACKDESC AtkDesc = m_pPlayerStats->Get_PlagueAttackDesc();

	m_AttackDescs.emplace_back(CPlayer::CLAW_R, AtkDesc);
}


void CPlayerState_Plunder::Plunder_PlagueWeapon()
{
	m_pPlayerStats->Update_PlunderSkill(m_pTargetEnemy->Get_SkillType());
}

void CPlayerState_Plunder::Plunder_Attack()
{
	m_pTargetEnemy->Hit(m_AttackDescs[0].second);
}

CPlayerState_Plunder* CPlayerState_Plunder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Plunder* pInstance = new CPlayerState_Plunder(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Plunder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Plunder::Free()
{
	__super::Free();
}
