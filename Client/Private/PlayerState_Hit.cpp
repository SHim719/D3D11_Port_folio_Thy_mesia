#include "PlayerState_Hit.h"

CPlayerState_Hit::CPlayerState_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Hit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_Avoid, PlayerState::State_Parry };


	return S_OK;
}

void CPlayerState_Hit::OnState_Start(void* pArg)
{
	m_pPlayer->Disable_NextState();
	m_pPlayer->Disable_Rotation();
	m_pPlayer->Inactive_AllWeaponColliders();

	ATTACKDESC* pAttackDesc = (ATTACKDESC*)pArg;
	
	ATTACKTYPE eAttackType = pAttackDesc->eAttackType;

	switch (eAttackType)
	{
	case WEAK:
		break;
	case NORMAL:
	{
		m_pModel->Change_Animation(Corvus_SD_HurtMFL + m_iHitDir);

		m_iHitDir = (m_iHitDir + 1) % 2;
		break;
	}
		
	case KNOCKBACK:
	{
		m_pModel->Change_Animation(Corvus_SD_KnockBackF);
		break;
	}
	}
}							   

void CPlayerState_Hit::OnGoing(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);


	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);

	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		m_pPlayer->Change_State((_uint)ePlayerState);
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
