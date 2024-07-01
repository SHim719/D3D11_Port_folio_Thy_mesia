#include "PlayerState_StealRush.h"

CPlayerState_StealRush::CPlayerState_StealRush(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_StealRush::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Avoid, PlayerState::State_Parry };

	return S_OK;
}

void CPlayerState_StealRush::OnState_Start(void* pArg)
{
	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_CanRotation(true);
	m_pPlayer->Set_Active_Claws(true);
	m_pPlayer->Set_Active_DefaultWeapons(false);

	m_pModel->Change_AnimationWithStartFrame(Corvus_Raven_ClawLong_ChargeFull, 40, 0.05f);
}

void CPlayerState_StealRush::Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);


	if (m_pPlayer->Can_Rotation() && false == m_pPlayer->Is_LockOn())
	{
		_vector vNewLook = Calc_MoveLook(true);

		if (0.f != vNewLook.m128_f32[0] || 0.f != vNewLook.m128_f32[1])
			Rotate_To_Look(vNewLook, fTimeDelta);
	}


	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);

}

void CPlayerState_StealRush::Late_Update(_float fTimeDelta)
{
	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		m_pPlayer->Change_State((_uint)ePlayerState);
}


void CPlayerState_StealRush::OnState_End()
{
	m_pPlayer->Set_Active_DefaultWeapons(true);
	m_pPlayer->Set_Active_Claws(false);
}


CPlayerState_StealRush* CPlayerState_StealRush::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_StealRush* pInstance = new CPlayerState_StealRush(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_StealRush"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_StealRush::Free()
{
	__super::Free();
}
