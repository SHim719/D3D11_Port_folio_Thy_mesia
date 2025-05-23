#include "PlayerState_Attack.h"
#include "Enemy.h"


CPlayerState_Attack::CPlayerState_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Attack::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_PlagueAttack, PlayerState::State_ChargeStart
		, PlayerState::State_Avoid, PlayerState::State_Parry, PlayerState::State_Healing };

	m_pModel->Bind_Func("Sound_Attack", bind(&CPlayerState_Attack::Play_AttackSound, this));
	m_pModel->Bind_Func("Active_RadialBlur_NormalAttack", bind(&CPlayerState_Attack::Active_RadialBlur, this));


	return S_OK;
}

void CPlayerState_Attack::OnState_Start(void* pArg)
{
	CEnemy* pExecutionEnemy = m_pPlayer->Get_ExecutionEnemy();
	if (pExecutionEnemy) {
		Decide_ExecutionState(pExecutionEnemy);
		return;
	}

	m_pPlayer->Set_CanNextAttack(false);
	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_CanRotation(true);


	Reset_AttackIdx();

	m_AttackDescs[0].second = m_pPlayerStats->Get_NormalAttackDesc();
	m_pPlayer->Update_AttackDesc();

	m_pModel->Change_Animation(_uint(Corvus_SD_LAttack1 + m_iNowComboCnt));
}

void CPlayerState_Attack::Update(_float fTimeDelta)
{
	if (m_pPlayer->Can_Rotation() && false == m_pPlayer->Is_LockOn())
	{
		_vector vNewLook = Calc_MoveLook(true);

		if (0.f != vNewLook.m128_f32[0] || 0.f != vNewLook.m128_f32[1])
			Rotate_To_Look(vNewLook, fTimeDelta);
	}

	if (false == m_pPlayer->Is_CollEnemy())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);

}

void CPlayerState_Attack::Late_Update(_float fTimeDelta)
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

void CPlayerState_Attack::OnState_End()
{
	m_iNowComboCnt = 0;

	m_pGameInstance->Inactive_RadialBlur(m_pPlayer->Get_ActorID(), 0.5f);
	m_pMain_Camera->End_DeltaFov(1.f);
}


void CPlayerState_Attack::Init_AttackDesc()
{
	m_AttackDescs.reserve(1);
	m_AttackDescs.emplace_back(CPlayer::SABER , ATTACKDESC());
}

void CPlayerState_Attack::Check_ExtraStateChange(PlayerState eState)
{
	switch (eState)
	{
	case PlayerState::State_PlagueAttack:
		Check_PlagueAttack();
		break;
	case PlayerState::State_Attack:
		Check_NextAttack();
		break;
	default:
		m_pPlayer->Change_State((_uint)eState);
		break;
	}
}

void CPlayerState_Attack::Decide_ExecutionState(CEnemy* pExecutionEnemy)
{
	switch (pExecutionEnemy->Get_ExecutionTag())
	{
	case CEnemy::DEFAULT:
		m_pPlayer->Change_State((_uint)PlayerState::State_Execution_Default, pExecutionEnemy);
		break;
	case CEnemy::JOKER:
		m_pPlayer->Change_State((_uint)PlayerState::State_Execution_Elite, pExecutionEnemy);
		m_pModel->Change_Animation(Corvus_VSJoker_Execution);
		break;
	case CEnemy::TWINBLADEKNIGHT:
		m_pPlayer->Change_State((_uint)PlayerState::State_Execution_Elite, pExecutionEnemy);
		m_pModel->Change_Animation(Corvus_VSHArmorLV1_Execution);
		break;
	case CEnemy::ODUR:
		m_pPlayer->Change_State((_uint)PlayerState::State_Execution_Elite, pExecutionEnemy);
		m_pModel->Change_Animation(Corvus_VSMagician_Execution);
		break;
	case CEnemy::URD:
		m_pPlayer->Change_State((_uint)PlayerState::State_Execution_Elite, pExecutionEnemy);
		m_pModel->Change_Animation(Corvus_VSUrd_Execution);
		break;
	}
}

void CPlayerState_Attack::Check_NextAttack()
{
	if (true == m_pPlayer->Can_NextAttack())
	{
		m_iNowComboCnt = (m_iNowComboCnt + 1) % m_iMaxComboCnt;
		OnState_Start(nullptr);
	}
}

void CPlayerState_Attack::Active_RadialBlur()
{
	RADIALBLUR_DESCS RadialDescs{};

	RadialDescs.pActor = m_pPlayer;
	RadialDescs.fBlurRadius = 10.f;
	RadialDescs.fBlurStrength = 2.f;

	m_pGameInstance->Update_BlurCenterWorld(m_pPlayer->Get_Center());
	m_pGameInstance->Active_RadialBlur(RadialDescs);
	m_pGameInstance->Inactive_RadialBlur(m_pPlayer->Get_ActorID(), 1.f);

	CMain_Camera::DELTAFOVYDESC DeltaFovYDesc{};
	DeltaFovYDesc.fReturnLerpTime = -1.f;
	DeltaFovYDesc.fTargetFovY = 75.f;
	DeltaFovYDesc.fTargetLerpTime = 0.2f;

	m_pMain_Camera->Add_DeltaFovYDesc(DeltaFovYDesc);
}

void CPlayerState_Attack::Play_AttackSound()
{
	wstring wstrSoundTag = L"Corvus_Attack" + to_wstring(m_iNowComboCnt + 1);
	PLAY_SOUND(wstrSoundTag, false, 1.f);
}


CPlayerState_Attack* CPlayerState_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Attack* pInstance = new CPlayerState_Attack(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Attack::Free()
{
	__super::Free();
}
