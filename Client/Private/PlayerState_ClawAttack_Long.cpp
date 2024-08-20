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
		PlayerState::State_Avoid, PlayerState::State_Parry, PlayerState::State_ChargeStart, PlayerState::State_Healing };

	m_pModel->Bind_Func("Shaking_Claw_Long", bind(&CMain_Camera::Play_CameraShake, m_pMain_Camera, "Shaking_Claw_Long"));
	m_pModel->Bind_Func("Sound_ClawLong", bind(&CGameInstance::Play, m_pGameInstance, L"Corvus_ClawLong1", false, 0.8f));

	return S_OK;
}

void CPlayerState_ClawAttack_Long::OnState_Start(void* pArg)
{
	Reset_AttackIdx();
	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_CanRotation(false);
	m_pPlayer->Set_Active_DefaultWeapons(false);
	m_pPlayer->Set_Active_Claws(true);
	m_pPlayer->Set_CanNextAttack(false);

	m_pPlayer->Update_AttackDesc();

	if (0 == m_iAttackCnt)
		m_pModel->Change_AnimationWithStartFrame(Corvus_Raven_ClawLong_L01, 27, 0.1f);
	else
		m_pModel->Change_AnimationWithStartFrame(Corvus_Raven_ClawLong_L02, 18, 0.1f);

	RADIALBLUR_DESCS RadialDescs{};
	RadialDescs.fBlurRadius = 8.f;
	RadialDescs.fBlurStrength = 1.5f;

	m_pGameInstance->Active_RadialBlur(RadialDescs);
	m_pGameInstance->Update_BlurCenterWorld(m_pPlayer->Get_Center());

	++m_iAttackCnt;
}

void CPlayerState_ClawAttack_Long::Update(_float fTimeDelta)
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

	m_pGameInstance->Inactive_RadialBlur(0.5f);

	m_iAttackCnt = 0;
}

void CPlayerState_ClawAttack_Long::Init_AttackDesc()
{
	m_AttackDescs.reserve(1);

	m_AttackDescs.emplace_back(CPlayer::CLAW_R, m_pPlayerStats->Get_PlagueAttackDesc());                                               
}

void CPlayerState_ClawAttack_Long::Check_ExtraStateChange(PlayerState eState)
{
	switch (eState)
	{
	case PlayerState::State_PlagueAttack:
		Check_PlagueAttack();
		break;

	case PlayerState::State_ChargeStart:
		if (m_pPlayer->Can_NextAttack() && 1 == m_iAttackCnt)
			OnState_Start(nullptr);
		break;
	default:
		m_pPlayer->Change_State((_uint)eState);
		break;
	}
}

void CPlayerState_ClawAttack_Long::Play_ClawLongSound()
{
	_int iRandNum = JoRandom::Random_Int(1, 2);
	wstring wstrSoundtag = L"Corvus_ClawLong" + to_wstring(iRandNum);

	PLAY_SOUND(wstrSoundtag, false, 0.8f);
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
