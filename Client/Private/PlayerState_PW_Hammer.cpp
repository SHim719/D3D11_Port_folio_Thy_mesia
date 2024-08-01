#include "PlayerState_PW_Hammer.h"

CPlayerState_PW_Hammer::CPlayerState_PW_Hammer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_PW_Hammer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_ChargeStart, PlayerState::State_Avoid, PlayerState::State_Parry };

	return S_OK;
}

void CPlayerState_PW_Hammer::OnState_Start(void* pArg)
{
	Reset_AttackIdx();

	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_Stanced(true);
	m_pPlayer->Set_Active_DefaultWeapons(false);
	m_pPlayer->Set_Active_Claws(false);

	m_pPlayer->Set_Active_Weapon(CPlayer::PW_HAMMER, true);
	m_pPlayer->Set_Adjust_NaviY(false);
	EFFECTMGR->Active_Effect("Effect_Corvus_PW_Hammer", &m_pPlayer->Get_EffectSpawnDesc());

	m_pPlayer->Update_AttackDesc();

	m_pModel->Change_Animation(Corvus_PW_Hammer_A);
}

void CPlayerState_PW_Hammer::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CPlayerState_PW_Hammer::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);

	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		m_pPlayer->Change_State((_uint)ePlayerState);

}

void CPlayerState_PW_Hammer::OnState_End()
{
	m_pPlayer->Set_Stanced(false);
	m_pPlayer->Set_Active_DefaultWeapons(true);
	m_pPlayer->Set_Adjust_NaviY(true);

	m_pPlayer->Set_Active_Weapon(CPlayer::PW_HAMMER, false);
	m_pPlayer->Set_Active_WeaponCollider(CPlayer::PW_HAMMER, false);

	EFFECTMGR->Inactive_Effect("Effect_Corvus_PW_Hammer");
	EFFECTMGR->Active_Effect("Effect_Corvus_PW_Hammer_Disappear", &m_pPlayer->Get_EffectSpawnDesc());
}

void CPlayerState_PW_Hammer::Init_AttackDesc()
{
	m_AttackDescs.reserve(1);

	ATTACKDESC AtkDesc = m_pPlayerStats->Get_PlagueAttackDesc();

	m_AttackDescs.emplace_back(CPlayer::PW_HAMMER, AtkDesc);
}



CPlayerState_PW_Hammer* CPlayerState_PW_Hammer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_PW_Hammer* pInstance = new CPlayerState_PW_Hammer(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_PW_Hammer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_PW_Hammer::Free()
{
	__super::Free();
}
