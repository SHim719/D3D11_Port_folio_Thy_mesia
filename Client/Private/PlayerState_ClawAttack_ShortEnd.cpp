#include "PlayerState_ClawAttack_ShortEnd.h"


CPlayerState_ClawAttack_ShortEnd::CPlayerState_ClawAttack_ShortEnd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_ClawAttack_ShortEnd::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_PlagueAttack,
		PlayerState::State_Avoid, PlayerState::State_Parry };

	return S_OK;
}

void CPlayerState_ClawAttack_ShortEnd::OnState_Start(void* pArg)
{
	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_CanRotation(true);
	m_pPlayer->Set_Active_DefaultWeapons(false);

	m_pModel->Change_Animation(Corvus_Raven_ClawParry_End, 0.05f);
}

void CPlayerState_ClawAttack_ShortEnd::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CPlayerState_ClawAttack_ShortEnd::Late_Update(_float fTimeDelta)
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


void CPlayerState_ClawAttack_ShortEnd::OnState_End()
{
	m_pPlayer->Set_Active_DefaultWeapons(true);
	m_pPlayer->Set_Active_Claws(false);
	
}

CPlayerState_ClawAttack_ShortEnd* CPlayerState_ClawAttack_ShortEnd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_ClawAttack_ShortEnd* pInstance = new CPlayerState_ClawAttack_ShortEnd(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_ClawAttack_ShortEnd"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ClawAttack_ShortEnd::Free()
{
	__super::Free();
}
