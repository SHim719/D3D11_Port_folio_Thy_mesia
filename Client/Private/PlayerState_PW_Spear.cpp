#include "PlayerState_PW_Spear.h"

CPlayerState_PW_Spear::CPlayerState_PW_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_PW_Spear::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_ChargeStart, PlayerState::State_Avoid, PlayerState::State_Parry,
		PlayerState::State_Healing };

	m_pModel->Bind_Func("Sound_PW_Spear", bind(&CGameInstance::Play, m_pGameInstance, L"PW_Spear", false, 1.f));
	m_pModel->Bind_Func("Active_RadialBlur_PW_Spear", bind(&CPlayerState_PW_Spear::Active_RadialBlur, this));

	return S_OK;
}

void CPlayerState_PW_Spear::OnState_Start(void* pArg)
{
	Reset_AttackIdx();

	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_Stanced(true);
	m_pPlayer->Set_Active_DefaultWeapons(false);
	m_pPlayer->Set_Active_Claws(false);

	m_pPlayer->Set_Active_Weapon(CPlayer::PW_SPEAR, true);
	EFFECTMGR->Active_Effect("Effect_Corvus_PW_Spear", &m_pPlayer->Get_EffectSpawnDesc());
	
	Reset_AttackDesc();

	m_pPlayer->Update_AttackDesc();

	m_pModel->Change_Animation(Corvus_PW_Halberds_B);
}

void CPlayerState_PW_Spear::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CPlayerState_PW_Spear::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);

	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		m_pPlayer->Change_State((_uint)ePlayerState);

}

void CPlayerState_PW_Spear::OnState_End()
{
	m_pPlayer->Set_Stanced(false);
	m_pPlayer->Set_Active_DefaultWeapons(true);

	m_pPlayer->Set_Active_Weapon(CPlayer::PW_SPEAR, false);
	m_pPlayer->Set_Active_WeaponCollider(CPlayer::PW_SPEAR, false);

	m_pGameInstance->Inactive_RadialBlur(m_pPlayer->Get_ActorID(), 0.5f);
	m_pMain_Camera->End_DeltaFov(1.f);

	EFFECTMGR->Inactive_Effect("Effect_Corvus_PW_Spear");
	EFFECTMGR->Active_Effect("Effect_Corvus_PW_Spear_Disappear", &m_pPlayer->Get_EffectSpawnDesc());

	if (m_pGameInstance->Is_Playing(L"PW_Spear"))
		m_pGameInstance->Stop(L"PW_Spear");
}

void CPlayerState_PW_Spear::Init_AttackDesc()
{
	m_AttackDescs.reserve(2);

	ATTACKDESC AtkDesc = m_pPlayerStats->Get_PlagueAttackDesc();

	m_AttackDescs.emplace_back(CPlayer::PW_SPEAR, AtkDesc);

	AtkDesc = m_pPlayerStats->Get_SkillAttackDesc(SKILLTYPE::SPEAR);

	m_AttackDescs.emplace_back(CPlayer::PW_SPEAR, AtkDesc);
}

void CPlayerState_PW_Spear::Reset_AttackDesc()
{
	m_AttackDescs[0].second =  m_pPlayerStats->Get_NormalAttackDesc();
	m_AttackDescs[1].second = m_pPlayerStats->Get_SkillAttackDesc(SKILLTYPE::SPEAR);
}

void CPlayerState_PW_Spear::Active_RadialBlur()
{
	RADIALBLUR_DESCS RadialDescs{};

	RadialDescs.pActor = m_pPlayer;
	RadialDescs.fBlurRadius = 10.f;
	RadialDescs.fBlurStrength = 3.f;

	m_pGameInstance->Update_BlurCenterWorld(m_pPlayer->Get_Center());
	m_pGameInstance->Active_RadialBlur(RadialDescs);
	m_pGameInstance->Inactive_RadialBlur(m_pPlayer->Get_ActorID(), 1.f);

	CMain_Camera::DELTAFOVYDESC DeltaFovYDesc{};
	DeltaFovYDesc.fReturnLerpTime = -1.f;
	DeltaFovYDesc.fTargetFovY = 80.f;
	DeltaFovYDesc.fTargetLerpTime = 0.2f;

	m_pMain_Camera->Add_DeltaFovYDesc(DeltaFovYDesc);

}

CPlayerState_PW_Spear* CPlayerState_PW_Spear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_PW_Spear* pInstance = new CPlayerState_PW_Spear(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_PW_Spear"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_PW_Spear::Free()
{
	__super::Free();
}
