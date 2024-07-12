#include "PlayerState_Sprint.h"


CPlayerState_Sprint::CPlayerState_Sprint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Sprint::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Idle, PlayerState::State_LockOn, PlayerState::State_Attack, PlayerState::State_PlagueAttack
		, PlayerState::State_ChargeStart, PlayerState::State_Avoid, PlayerState::State_Parry };
	
	m_PossibleStates.reserve(m_PossibleStates.size() + 2);
	m_PossibleStates.emplace_back(PlayerState::State_Idle);
	m_PossibleStates.emplace_back(PlayerState::State_LockOn);

	m_bCanClimb = true;

	return S_OK;
}

void CPlayerState_Sprint::OnState_Start(void* pArg)
{
	if (m_pPlayer->Is_LockOn())
	{
		m_pPlayer->Change_State((_uint)PlayerState::State_LockOn);
		return;
	}

	m_pPlayer->Set_CanNextState(true);
	m_pPlayer->Set_CanRotation(true);

	m_pOwnerTransform->Set_Speed(m_fSprintSpeed);
	m_pModel->Change_Animation(Corvus_SD1_Sprint);
}

void CPlayerState_Sprint::Update(_float fTimeDelta)
{
	_vector vNewLook = Calc_MoveLook(true);

	if (0.f != vNewLook.m128_f32[0] || 0.f != vNewLook.m128_f32[1])
	{
		m_pOwnerTransform->Set_MoveLook(vNewLook);

		Rotate_To_Look(vNewLook, fTimeDelta);

		m_pOwnerTransform->Go_Dir(vNewLook, fTimeDelta, m_pNavigation);
	}
}

void CPlayerState_Sprint::Late_Update(_float fTimeDelta)
{
	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		Check_ExtraStateChange(ePlayerState);
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
