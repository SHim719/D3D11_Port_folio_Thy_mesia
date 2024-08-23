#include "PlayerState_PW_TwinBlade.h"

CPlayerState_PW_TwinBlade::CPlayerState_PW_TwinBlade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_PW_TwinBlade::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModel->Bind_Func("Sound_PW_TwinBlade_Slash_Final", bind(&CGameInstance::Play, m_pGameInstance, L"PW_TwinBlade_Slash_Final", false, 1.f));

	m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_ChargeStart, PlayerState::State_Avoid, PlayerState::State_Parry
	, PlayerState::State_Healing };

	return S_OK;
}

void CPlayerState_PW_TwinBlade::OnState_Start(void* pArg)
{
	Reset_AttackIdx();

	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_Stanced(true);
	m_pPlayer->Set_Active_DefaultWeapons(false);
	m_pPlayer->Set_Active_Claws(false);

	m_pPlayer->Set_Active_Weapon(CPlayer::PW_TWINBLADE_L, true);
	m_pPlayer->Set_Active_Weapon(CPlayer::PW_TWINBLADE_R, true);
	EFFECTMGR->Active_Effect("Effect_Corvus_PW_TwinBlade", &m_pPlayer->Get_EffectSpawnDesc());
	PLAY_SOUND(L"PW_TwinBlade", false, 0.7f);

	Reset_AttackDesc();
	m_pPlayer->Update_AttackDesc();

	m_pModel->Change_Animation(Corvus_PW_TwinSwords_1);
}

void CPlayerState_PW_TwinBlade::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CPlayerState_PW_TwinBlade::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);

	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		m_pPlayer->Change_State((_uint)ePlayerState);

}

void CPlayerState_PW_TwinBlade::OnState_End()
{
	m_pPlayer->Set_Stanced(false);
	m_pPlayer->Set_Active_DefaultWeapons(true);

	m_pPlayer->Set_Active_Weapon(CPlayer::PW_TWINBLADE_L, false);
	m_pPlayer->Set_Active_Weapon(CPlayer::PW_TWINBLADE_R, false);

	m_pPlayer->Set_Active_WeaponCollider(CPlayer::PW_TWINBLADE_L, false);
	m_pPlayer->Set_Active_WeaponCollider(CPlayer::PW_TWINBLADE_R, false);

	EFFECTMGR->Inactive_Effect("Effect_Corvus_PW_TwinBlade");
	EFFECTMGR->Active_Effect("Effect_Corvus_PW_TwinBlade_Disappear", &m_pPlayer->Get_EffectSpawnDesc());

	if (m_pGameInstance->Is_Playing(L"PW_TwinBlade"))
		m_pGameInstance->Stop(L"PW_TwinBlade");
}

void CPlayerState_PW_TwinBlade::Init_AttackDesc()
{
	m_AttackDescs.reserve(3);

	ATTACKDESC AtkDesc = m_pPlayerStats->Get_PlagueAttackDesc();
	AtkDesc.iDamage = AtkDesc.iPlagueDamage;

	m_AttackDescs.emplace_back(CPlayer::PW_TWINBLADE_L, AtkDesc);
	m_AttackDescs.emplace_back(CPlayer::PW_TWINBLADE_R, AtkDesc);
	m_AttackDescs.emplace_back(CPlayer::PW_TWINBLADE_R, AtkDesc);
}

void CPlayerState_PW_TwinBlade::Reset_AttackDesc()
{
	m_AttackDescs[0].second = m_AttackDescs[1].second = m_AttackDescs[2].second = m_pPlayer->Get_PlayerStats()->Get_SkillAttackDesc(SKILLTYPE::TWINBLADE);
	m_AttackDescs[2].second.iDamage *= 2;
	m_AttackDescs[2].second.iPlagueDamage *= 2;
}

CPlayerState_PW_TwinBlade* CPlayerState_PW_TwinBlade::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_PW_TwinBlade* pInstance = new CPlayerState_PW_TwinBlade(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_PW_TwinBlade"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_PW_TwinBlade::Free()
{
	__super::Free();
}
