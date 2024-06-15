#include "PlayerState_Idle.h"


CPlayerState_Idle::CPlayerState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Idle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Jog, PlayerState::State_Sprint, PlayerState::State_LockOn, PlayerState::State_Attack,
	PlayerState::State_Avoid, PlayerState::State_Parry };

	return S_OK;
}

void CPlayerState_Idle::OnState_Start(void* pArg)
{
	m_pPlayer->Enable_NextState();
	m_pPlayer->Enable_Rotation();

	m_pModel->Change_Animation(Corvus_SD_Idle);
}

void CPlayerState_Idle::OnGoing(_float fTimeDelta)
{
	
	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		m_pPlayer->Change_State((_uint)ePlayerState);
}

void CPlayerState_Idle::OnState_End()
{
}


CPlayerState_Idle* CPlayerState_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Idle* pInstance = new CPlayerState_Idle(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Idle::Free()
{
	__super::Free();
}
