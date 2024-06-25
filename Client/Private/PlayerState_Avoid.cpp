#include "PlayerState_Avoid.h"


CPlayerState_Avoid::CPlayerState_Avoid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Avoid::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fRotRate = 30.f;
	m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_Parry, PlayerState::State_Avoid};
	return S_OK;
}

void CPlayerState_Avoid::OnState_Start(void* pArg)
{	
	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_CanRotation(true);

	m_pPlayer->Set_Invincible(true);

	Decide_Animation();
}

void CPlayerState_Avoid::OnGoing(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
   		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
		return;
	}

	if (m_pPlayer->Can_Rotation() && false == m_pPlayer->Is_LockOn())
		Rotate_To_Look(m_pOwnerTransform->Get_MoveLook(), fTimeDelta);
	

	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);

	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
	{
		if (PlayerState::State_Avoid == ePlayerState)
		{
			if (m_bOneMore)
			{
				m_bOneMore = false;
				m_pPlayer->Change_State((_uint)ePlayerState);
			}
		}
		else
			m_pPlayer->Change_State((_uint)ePlayerState);

	}
		
}

void CPlayerState_Avoid::OnState_End()
{
	m_vMoveAxis.x = 0.f;
	m_vMoveAxis.y = 0.f;
}

void CPlayerState_Avoid::OnHit(void* pArg)
{
	if (m_pPlayer->Is_Invincible())
		m_bOneMore = true;
	//else Hit;
}

void CPlayerState_Avoid::Decide_Animation()
{
	_vector vMoveLook = Calc_MoveLook(true);

	if (0.f != vMoveLook.m128_f32[0] || 0.f != vMoveLook.m128_f32[1])
		m_pOwnerTransform->Set_MoveLook(vMoveLook);

	if (m_pPlayer->Is_LockOn())
	{
		m_vMoveAxis.y = m_vMoveAxis.x != 0.f ? 0.f : m_vMoveAxis.y;

		if (m_vMoveAxis.x > 0.f)
			m_pModel->Change_Animation(Corvus_SD_AvoidShortMag_R);
		else if (m_vMoveAxis.x < 0.f)
			m_pModel->Change_Animation(Corvus_SD_AvoidShortMag_L);
		else if (m_vMoveAxis.y >= 0.f)
			m_pModel->Change_Animation(Corvus_SD_AvoidShortMag_F);
		else
			m_pModel->Change_Animation(Corvus_SD_AvoidShortMag_B);
	}
	else
	{
		m_pModel->Change_Animation(Corvus_SD_AvoidF);
	}
}


CPlayerState_Avoid* CPlayerState_Avoid::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Avoid* pInstance = new CPlayerState_Avoid(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Avoid"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Avoid::Free()
{
	__super::Free();
}
