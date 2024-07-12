#include "PlayerState_Climb_End.h"

CPlayerState_Climb_End::CPlayerState_Climb_End(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Climb_End::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { };

	return S_OK;
}

void CPlayerState_Climb_End::OnState_Start(void* pArg)
{
	_int* pDir = (_int*)pArg;
	m_fMoveRate = 1.f;

	if (KEY_PUSHING(eKeyCode::W))
	{
		if (pDir)
			m_pModel->Change_Animation(Corvus_SD_Ladder_Climb_R_UP_End);
		else
			m_pModel->Change_Animation(Corvus_SD_Ladder_Climb_L_UP_End);

		m_bIsUp = true;
	}
	else
	{
		if (0 == m_pPlayer->Get_PlayerLadderHeight())
			m_fMoveRate = 0.52f;

		if (pDir)
			m_pModel->Change_Animation(Corvus_SD_Ladder_Climb_R_Down_End);
		else
			m_pModel->Change_Animation(Corvus_SD_Ladder_Climb_L_Down_End);

		m_bIsUp = false;
	}
}

void CPlayerState_Climb_End::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos() * m_fMoveRate);
}

void CPlayerState_Climb_End::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);

}

void CPlayerState_Climb_End::OnState_End()
{
	if (true == m_bIsUp && true == m_pPlayer->Is_ClimbStartDown())
		m_pNavigation->Set_CurrentIdx(m_pPlayer->Get_TargetNaviIdx());
	else if (false == m_bIsUp && false == m_pPlayer->Is_ClimbStartDown())
		m_pNavigation->Set_CurrentIdx(m_pPlayer->Get_TargetNaviIdx());
}

CPlayerState_Climb_End* CPlayerState_Climb_End::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Climb_End* pInstance = new CPlayerState_Climb_End(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Climb_End"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Climb_End::Free()
{
	__super::Free();
}
