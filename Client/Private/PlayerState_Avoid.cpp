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

	m_PossibleStates = {  PlayerState::State_Jog, PlayerState::State_LockOn
		, PlayerState::State_Attack, PlayerState::State_PlagueAttack, PlayerState::State_ChargeStart
		, PlayerState::State_Avoid, PlayerState::State_Parry };

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
	if (m_pPlayer->Can_Rotation() && false == m_pPlayer->Is_LockOn())
		Rotate_To_Look(m_pOwnerTransform->Get_MoveLook(), fTimeDelta);
	
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CPlayerState_Avoid::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
		return;
	}

	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		Check_ExtraStateChange(ePlayerState);

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
	switch (eState)
	{
	case PlayerState::State_PlagueAttack:
		Check_PlagueAttack();
		break;
	case PlayerState::State_Avoid:
		Check_AvoidOneMore();
		break;
	default:
		m_pPlayer->Change_State((_uint)eState);
		break;
	}
}

void CPlayerState_Avoid::Check_AvoidOneMore()
{
	if (m_bOneMore)
	{
		m_bOneMore = false;
		m_pPlayer->Change_State((_uint)PlayerState::State_Avoid);
	}
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
