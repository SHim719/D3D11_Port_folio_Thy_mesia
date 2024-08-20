#include "PlayerState_Hit.h"

CPlayerState_Hit::CPlayerState_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Hit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_PlagueAttack
		, PlayerState::State_ChargeStart, PlayerState::State_Avoid, PlayerState::State_Parry, PlayerState::State_Healing };

	return S_OK;
}

void CPlayerState_Hit::OnState_Start(void* pArg)
{
	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_CanRotation(false);
	m_pPlayer->Inactive_AllWeaponColliders();

	ATTACKDESC* pAttackDesc = (ATTACKDESC*)pArg;

	if (false == m_pPlayer->Is_LockOn() && nullptr != pAttackDesc->pAttacker)
		m_pOwnerTransform->LookAt2D(pAttackDesc->pAttacker->Get_Transform()->Get_Position());

	switch (pAttackDesc->eEnemyAttackType)
	{
	case WEAK:
		m_pModel->Change_Animation(Corvus_SD_HurtSL + m_iHitDir);
		m_iHitDir = (m_iHitDir + 1) % 2;
		break;
	case NORMAL:
		m_pModel->Change_Animation(Corvus_SD_HurtMFL + m_iHitDir);
		m_iHitDir = (m_iHitDir + 1) % 2;
		break;
	case SEMIKNOCKBACK:
		m_pModel->Change_Animation(Corvus_SD_HurtLF);
		break;
	case KNOCKBACK:
		m_pModel->Change_Animation(Corvus_SD_KnockBackF);
		break;
	case KNOCKDOWN:
		m_pPlayer->Change_State((_uint)PlayerState::State_KnockDown);
		break;
	case AIR:
		m_pPlayer->Change_State((_uint)PlayerState::State_FallDown);
		break;
	case VERY_BIG_HIT:
		m_pModel->Change_Animation(Corvus_SD_HurtXXLF);
		break;
	}
}							   

void CPlayerState_Hit::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CPlayerState_Hit::Late_Update(_float fTimeDelta)
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

void CPlayerState_Hit::OnState_End()
{
	m_iHitDir = 0;
}


CPlayerState_Hit* CPlayerState_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Hit* pInstance = new CPlayerState_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Hit::Free()
{
	__super::Free();
}
