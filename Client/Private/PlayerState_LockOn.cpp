#include "PlayerState_LockOn.h"

CPlayerState_LockOn::CPlayerState_LockOn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_LockOn::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Idle, PlayerState::State_Attack };

	return S_OK;
}

void CPlayerState_LockOn::OnState_Start(void* pArg)
{
	m_pOwnerTransform->Set_Speed(m_fJogSpeed);
}

void CPlayerState_LockOn::OnGoing(_float fTimeDelta)
{
	_vector vNewLook = Calc_MoveLook(false);

	Change_WalkAnimation();

	m_pOwnerTransform->Go_Dir(vNewLook, fTimeDelta);

	Decide_State();
}

void CPlayerState_LockOn::OnState_End()
{
	m_vPrevMoveAxis.x = 0.f;
	m_vPrevMoveAxis.y = 0.f;
}

void CPlayerState_LockOn::Change_WalkAnimation()
{
	if (m_vPrevMoveAxis.x == m_vMoveAxis.x && m_vPrevMoveAxis.y == m_vMoveAxis.y)
		return;

	if (1.f == m_vMoveAxis.x)
	{
		if (-1.f == m_vMoveAxis.y)
			m_pModel->Change_Animation(Corvus_SD_RunBR);
		else if (0.f == m_vMoveAxis.y)
			m_pModel->Change_Animation(Corvus_SD_RunR, 0.f);
		else
			m_pModel->Change_Animation(Corvus_SD_RunFR);
	}
	else if (-1.f == m_vMoveAxis.x)
	{
		if (-1.f == m_vMoveAxis.y)
			m_pModel->Change_Animation(Corvus_SD_RunBL);
		else if (0.f == m_vMoveAxis.y)
			m_pModel->Change_Animation(Corvus_SD_RunL, 0.f);
		else
			m_pModel->Change_Animation(Corvus_SD_RunFL);
	}
	else
	{
		if (-1.f == m_vMoveAxis.y)
			m_pModel->Change_Animation(Corvus_SD_RunB);
		else if (1.f == m_vMoveAxis.y)
			m_pModel->Change_Animation(Corvus_SD_RunF_24);
	}

	m_vPrevMoveAxis.x = m_vMoveAxis.x;
	m_vPrevMoveAxis.y = m_vMoveAxis.y;
}

CPlayerState_LockOn* CPlayerState_LockOn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_LockOn* pInstance = new CPlayerState_LockOn(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_LockOn"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_LockOn::Free()
{
}
