#include "PlayerState_ChargeComplete.h"

CPlayerState_ChargeComplete::CPlayerState_ChargeComplete(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_ChargeComplete::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_PlagueAttack,
		PlayerState::State_Avoid, PlayerState::State_Parry, PlayerState::State_PlunderRush };

	return S_OK;
}

void CPlayerState_ChargeComplete::OnState_Start(void* pArg)
{
	m_pPlayer->Set_CanNextState(true);
	m_pPlayer->Set_CanRotation(true);
	m_pPlayer->Set_Active_DefaultWeapons(false);
	m_pPlayer->Set_Active_Claws(true);

	m_pModel->Change_Animation(Corvus_Raven_ClawCommon_ChargeStep);
}

void CPlayerState_ChargeComplete::Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_ChargeLoop);


	if (m_pPlayer->Can_Rotation() && false == m_pPlayer->Is_LockOn())
	{
		_vector vNewLook = Calc_MoveLook(true);

		if (0.f != vNewLook.m128_f32[0] || 0.f != vNewLook.m128_f32[1])
			Rotate_To_Look(vNewLook, fTimeDelta);
	}

	
}

void CPlayerState_ChargeComplete::Late_Update(_float fTimeDelta)
{
	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		Check_ExtraStateChange(ePlayerState);
}


void CPlayerState_ChargeComplete::OnState_End()
{
	m_pPlayer->Set_Active_DefaultWeapons(true);
	m_pPlayer->Set_Active_Claws(false);
}


CPlayerState_ChargeComplete* CPlayerState_ChargeComplete::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_ChargeComplete* pInstance = new CPlayerState_ChargeComplete(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_ChargeComplete"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ChargeComplete::Free()
{
	__super::Free();
}
