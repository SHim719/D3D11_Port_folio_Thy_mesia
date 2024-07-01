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
	m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_ChargeStart, PlayerState::State_Parry, PlayerState::State_Avoid};
	return S_OK;
}

void CPlayerState_Avoid::OnState_Start(void* pArg)
{	
	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_CanRotation(true);

	m_pPlayer->Set_Invincible(true);

	Decide_Animation();
}

void CPlayerState_Avoid::Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
   		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
		return;
	}

	if (m_pPlayer->Can_Rotation() && false == m_pPlayer->Is_LockOn())
		Rotate_To_Look(m_pOwnerTransform->Get_MoveLook(), fTimeDelta);
	

	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
	
}

void CPlayerState_Avoid::Late_Update(_float fTimeDelta)
{
	PlayerState ePlayerState = Decide_State();
	

}

void CPlayerState_Avoid::OnState_End()
{
	m_vMoveAxis.x = 0.f;
	m_vMoveAxis.y = 0.f;
}

void CPlayerState_Avoid::OnHit(const ATTACKDESC& AttackDesc)
{
	if (m_pPlayer->Is_Invincible())
		m_bOneMore = true;
	//else Hit;
}

void CPlayerState_Avoid::Check_ExtraStateChange(PlayerState eState)
{
	if (PlayerState::State_End == eState)
		return;

	if (PlayerState::State_Avoid == eState && m_bOneMore)
	{
		m_bOneMore = false;
		m_pPlayer->Change_State((_uint)eState);
	}
	else
		m_pPlayer->Change_State((_uint)eState);
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
