#include "PlayerState_Parried.h"

CPlayerState_Parried::CPlayerState_Parried(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Parried::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_PlagueAttack
	//	, PlayerState::State_ChargeStart, PlayerState::State_Avoid, PlayerState::State_Parry };

	return S_OK;
}

void CPlayerState_Parried::OnState_Start(void* pArg)
{
	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_CanRotation(false);
	m_pPlayer->Inactive_AllWeaponColliders();

	m_pModel->Change_Animation(Corvus_SD_Rebound_R, 0.f);
}

void CPlayerState_Parried::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CPlayerState_Parried::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
		return;
	}
}

void CPlayerState_Parried::OnState_End()
{

	//PlayerState ePlayerState = Decide_State();
	//if (PlayerState::State_End != ePlayerState)
	//	Check_ExtraStateChange(ePlayerState);	
}

CPlayerState_Parried* CPlayerState_Parried::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Parried* pInstance = new CPlayerState_Parried(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Parried"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Parried::Free()
{
	__super::Free();
}
