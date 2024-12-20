#include "PlayerState_Jog.h"


CPlayerState_Jog::CPlayerState_Jog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Jog::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Idle, PlayerState::State_Sprint, 
		PlayerState::State_LockOn, PlayerState::State_Attack, PlayerState::State_PlagueAttack, PlayerState::State_ChargeStart
		,PlayerState::State_Avoid, PlayerState::State_Parry, PlayerState::State_Healing };

	m_bCanClimb = true;

	return S_OK;
}

void CPlayerState_Jog::OnState_Start(void* pArg)
{
	//if (m_pPlayer->Is_LockOn())
	//{
	//	m_pPlayer->Change_State((_uint)PlayerState::State_LockOn);
	//	return;
	//}
	//
	//if (Check_StateChange(PlayerState::State_Sprint))
	//{
	//	m_pPlayer->Change_State((_uint)PlayerState::State_Sprint);
	//	return;
	//}

	m_pPlayer->Set_CanNextState(true);
	m_pPlayer->Set_CanRotation(true);

	m_pModel->Change_Animation(Corvus_SD_RunF_24);
	m_pOwnerTransform->Set_Speed(m_fJogSpeed);
}

void CPlayerState_Jog::Update(_float fTimeDelta)
{
	_vector vNewLook = Calc_MoveLook(true);

	if (0.f != vNewLook.m128_f32[0] || 0.f != vNewLook.m128_f32[1])
	{
		m_pOwnerTransform->Set_MoveLook(vNewLook);

		Rotate_To_Look(vNewLook, fTimeDelta);

		m_pOwnerTransform->Go_Dir(vNewLook, fTimeDelta, m_pNavigation);
	}
}

void CPlayerState_Jog::Late_Update(_float fTimeDelta)
{
	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		Check_ExtraStateChange(ePlayerState);
}

void CPlayerState_Jog::OnState_End()
{
}


CPlayerState_Jog* CPlayerState_Jog::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Jog* pInstance = new CPlayerState_Jog(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Jog"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Jog::Free()
{
	__super::Free();
}
