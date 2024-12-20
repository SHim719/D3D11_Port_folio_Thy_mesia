#include "PlayerState_PW_Axe.h"

CPlayerState_PW_Axe::CPlayerState_PW_Axe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_PW_Axe::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_ChargeStart, PlayerState::State_Avoid, PlayerState::State_Parry
	, PlayerState::State_Healing };

	m_pModel->Bind_Func("Shaking_PW_Axe", bind(&CMain_Camera::Play_CameraShake, m_pMain_Camera, "Shaking_PW_Axe"));
	m_pModel->Bind_Func("Sound_PW_Axe_Impact", bind(&CGameInstance::Play, m_pGameInstance, L"PW_Axe_Impact", false, 0.7f));

	return S_OK;
}

void CPlayerState_PW_Axe::OnState_Start(void* pArg)
{
	Reset_AttackIdx();

	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_Stanced(true);
	m_pPlayer->Set_Active_DefaultWeapons(false);
	m_pPlayer->Set_Active_Claws(false);

	m_pPlayer->Set_Active_Weapon(CPlayer::PW_AXE, true);
	//m_pPlayer->Set_Adjust_NaviY(false);

	Reset_AttackDesc();
	m_pPlayer->Update_AttackDesc();

	EFFECTMGR->Active_Effect("Effect_Corvus_PW_Axe", &m_pPlayer->Get_EffectSpawnDesc());
	PLAY_SOUND(L"PW_Active", false, 1.f);
	
	m_pModel->Change_Animation(Corvus_PW_Axe);
}

void CPlayerState_PW_Axe::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CPlayerState_PW_Axe::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);

	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		m_pPlayer->Change_State((_uint)ePlayerState);

}

void CPlayerState_PW_Axe::OnState_End()
{
	m_pPlayer->Set_Stanced(false);
	m_pPlayer->Set_Active_DefaultWeapons(true);
	m_pPlayer->Set_Adjust_NaviY(true);

	m_pPlayer->Set_Active_Weapon(CPlayer::PW_AXE, false);
	m_pPlayer->Set_Active_WeaponCollider(CPlayer::PW_AXE, false);

	EFFECTMGR->Inactive_Effect("Effect_Corvus_PW_Axe");
	EFFECTMGR->Active_Effect("Effect_Corvus_PW_Axe_Disappear", &m_pPlayer->Get_EffectSpawnDesc());
}

void CPlayerState_PW_Axe::Init_AttackDesc()
{
	m_AttackDescs.reserve(1);

	ATTACKDESC AtkDesc = m_pPlayerStats->Get_PlagueAttackDesc();

	m_AttackDescs.emplace_back(CPlayer::PW_AXE, AtkDesc);
}

void CPlayerState_PW_Axe::Reset_AttackDesc()
{
	m_AttackDescs[0].second = m_pPlayer->Get_PlayerStats()->Get_SkillAttackDesc(SKILLTYPE::AXE);
}

CPlayerState_PW_Axe* CPlayerState_PW_Axe::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_PW_Axe* pInstance = new CPlayerState_PW_Axe(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_PW_Axe"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_PW_Axe::Free()
{
	__super::Free();
}
