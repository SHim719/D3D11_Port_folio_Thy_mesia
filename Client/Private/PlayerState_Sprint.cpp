#include "PlayerState_Sprint.h"


CPlayerState_Sprint::CPlayerState_Sprint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Sprint::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Idle, PlayerState::State_LockOn, PlayerState::State_Attack };


	return S_OK;
}

void CPlayerState_Sprint::OnState_Start(void* pArg)
{
	m_pOwnerTransform->Set_Speed(m_fSprintSpeed);
	m_pModel->Change_Animation(Corvus_SD1_Sprint);
}

void CPlayerState_Sprint::OnGoing(_float fTimeDelta)
{
	_vector vNewLook = Calc_MoveLook(true);

	if (0.f != vNewLook.m128_f32[0] || 0.f != vNewLook.m128_f32[1])
	{
		m_pOwnerTransform->Set_MoveLook(vNewLook);

		Rotate_To_Look(vNewLook, fTimeDelta);

		m_pOwnerTransform->Go_Dir(vNewLook, fTimeDelta);
	}


	Decide_State();
}

void CPlayerState_Sprint::OnState_End()
{
	m_pOwnerTransform->Set_Speed(m_fJogSpeed);
}

CPlayerState_Sprint* CPlayerState_Sprint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Sprint* pInstance = new CPlayerState_Sprint(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Sprint"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Sprint::Free()
{
	__super::Free();
}
