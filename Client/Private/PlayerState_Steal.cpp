#include "PlayerState_Steal.h"

CPlayerState_Steal::CPlayerState_Steal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Steal::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Avoid, PlayerState::State_Parry };

	return S_OK;
}

void CPlayerState_Steal::OnState_Start(void* pArg)
{
	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_CanRotation(true);

	m_pModel->Change_Animation(Corvus_Raven_ClawCommonV2_ChargeStart);
}

void CPlayerState_Steal::Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_ChargeLoop);


	if (m_pPlayer->Can_Rotation() && false == m_pPlayer->Is_LockOn())
	{
		_vector vNewLook = Calc_MoveLook(true);

		if (0.f != vNewLook.m128_f32[0] || 0.f != vNewLook.m128_f32[1])
			Rotate_To_Look(vNewLook, fTimeDelta);
	}

	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		m_pPlayer->Change_State((_uint)ePlayerState);
}


void CPlayerState_Steal::OnState_End()
{

}


CPlayerState_Steal* CPlayerState_Steal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Steal* pInstance = new CPlayerState_Steal(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Steal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Steal::Free()
{
	__super::Free();
}
