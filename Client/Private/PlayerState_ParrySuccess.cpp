#include "PlayerState_ParrySuccess.h"


CPlayerState_ParrySuccess::CPlayerState_ParrySuccess(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_ParrySuccess::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Jog, PlayerState::State_Attack, PlayerState::State_PlagueAttack, PlayerState::State_ChargeStart,
		PlayerState::State_Avoid, PlayerState::State_Parry, PlayerState::State_Healing };

	m_pModel->Bind_Func("Reset_PushStrength", bind(&CPlayerState_ParrySuccess::Set_PushStrength, this, 1.f));

    return S_OK;
}

void CPlayerState_ParrySuccess::OnState_Start(void* pArg)
{
	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_CanRotation(false);
	//m_pPlayer->Set_Invincible(true);
	m_pPlayer->Set_EnableJog(false);
	m_pPlayer->Set_CanParry(true);

	PARRY_DESC* Desc = (PARRY_DESC*)pArg;

	m_iParryDir = Desc->iParryDir;
	if (nullptr != Desc->AttackDesc.pAttacker)
	{
		ATTACKDESC ParryAttackDesc = m_pPlayerStats->Get_NormalAttackDesc();
		ParryAttackDesc.iPlagueDamage = ParryAttackDesc.iDamage;
		ParryAttackDesc.ePlayerAttackType = PARRY;
		Desc->AttackDesc.pAttacker->Hit(ParryAttackDesc);
		m_pOwnerTransform->LookAt2D(Desc->AttackDesc.pAttacker->Get_Transform()->Get_Position());
	}
	
	EFFECTMGR->Active_Effect("Effect_Corvus_Parry_Success", &m_pPlayer->Get_EffectSpawnDesc());

	Play_CameraShake(Desc->AttackDesc);


	_int iRandNum = JoRandom::Random_Int(0, 1);

	if (0 == m_iParryDir)
		m_pModel->Change_Animation(Corvus_SD_ParryDeflect_L + iRandNum, 0.f);
	else
		m_pModel->Change_Animation(Corvus_SD_ParryDeflect_R + iRandNum, 0.f);

	iRandNum = JoRandom::Random_Int(1, 3);
	wstring wstrSoundTag = L"Parry_Success" + to_wstring(iRandNum);

	PLAY_SOUND(wstrSoundTag, false, 1.f);
}

void CPlayerState_ParrySuccess::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos() * m_fPushStrength, m_pNavigation);

}

void CPlayerState_ParrySuccess::Late_Update(_float fTimeDelta)
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

void CPlayerState_ParrySuccess::OnState_End()
{
	m_pPlayer->Set_EnableJog(true);
}

void CPlayerState_ParrySuccess::OnHit(const ATTACKDESC& AttackDesc)
{
	if (false == m_pPlayer->Can_NextState() && KEY_PUSHING(eKeyCode::F))
	{
		PARRY_DESC ParryDesc{};
		ParryDesc.iParryDir = ++m_iParryDir;
		ParryDesc.AttackDesc = AttackDesc;

		OnState_Start(&ParryDesc);
	}

	else
	{
		__super::OnHit(AttackDesc);
	}
		
}

void CPlayerState_ParrySuccess::Play_CameraShake(const ATTACKDESC& AttackDesc)
{
	switch (AttackDesc.eEnemyAttackType)
	{
	case Client::NORMAL:
		m_pMain_Camera->Play_CameraShake("Shaking_Parry_Normal");
		m_fPushStrength = 1.f;
		break;
	case Client::SEMIKNOCKBACK:
	case Client::KNOCKBACK:
	case Client::KNOCKDOWN:
		m_pMain_Camera->Play_CameraShake("Shaking_Parry_Big");
		m_fPushStrength = 4.f;
		break;
	}
}


CPlayerState_ParrySuccess* CPlayerState_ParrySuccess::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_ParrySuccess* pInstance = new CPlayerState_ParrySuccess(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_ParrySuccess"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ParrySuccess::Free()
{
	__super::Free();
}
