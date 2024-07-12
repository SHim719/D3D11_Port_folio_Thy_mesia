#include "PlayerState_Base.h"
#include "Main_Camera.h"


CPlayerState_Base::CPlayerState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Base::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pPlayer = (CPlayer*)pArg;

	m_pMain_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_MainCamera());
	if (nullptr == m_pMain_Camera)
		return E_FAIL;

	m_pPlayerStats = m_pPlayer->Get_PlayerStats();

	Init_AttackDesc();

	return S_OK;
}

void CPlayerState_Base::OnState_Start(void* pArg)
{
}

void CPlayerState_Base::Update(_float fTimeDelta)
{
}

void CPlayerState_Base::Late_Update(_float fTimeDelta)
{
}

void CPlayerState_Base::OnState_End()
{
}

void CPlayerState_Base::OnHit(const ATTACKDESC& AttackDesc)
{
	ENEMYATTACKTYPE eEnemyAttackType = AttackDesc.eEnemyAttackType;

	// 체력감소 죽음상태 체크
	if (0 == m_pPlayer->Take_Damage(AttackDesc))
		//Death
		int x = 10;
	else
		m_pPlayer->Change_State((_uint)PlayerState::State_Hit, &eEnemyAttackType);
}

_vector CPlayerState_Base::Calc_MoveLook(_bool IsCamOriented)
{
	_vector vLook, vRight;
	if (IsCamOriented)
	{
		CTransform* pCameraTransform = m_pMain_Camera->Get_Transform();
		vLook = pCameraTransform->Get_GroundLook();
		vRight = pCameraTransform->Get_GroundRight();
	}
	else
	{
		vLook = m_pOwnerTransform->Get_GroundLook();
		vRight = m_pOwnerTransform->Get_GroundRight();
	}

	_vector vNewLook = XMVectorZero();

	m_vMoveAxis.x = 0.f;
	m_vMoveAxis.y = 0.f;

	if (KEY_PUSHING(eKeyCode::W))
	{
		vNewLook += vLook;
		m_vMoveAxis.y = 1.f;
	}

	if (KEY_PUSHING(eKeyCode::S))
	{
		vNewLook -= vLook;
		m_vMoveAxis.y = -1.f;
	}

	if (KEY_PUSHING(eKeyCode::D))
	{
		vNewLook += vRight;
		m_vMoveAxis.x = 1.f;
	}

	if (KEY_PUSHING(eKeyCode::A))
	{
		vNewLook -= vRight;
		m_vMoveAxis.x = -1.f;
	}
		

	if (XMVectorGetX(XMVector3Length(vNewLook)) < FLT_EPSILON)
		return XMVectorZero();

	return XMVector3Normalize(vNewLook);
}

void CPlayerState_Base::Rotate_To_Look(_vector vNewLook, _float fTimeDelta)
{
	_vector vPlayerLook = m_pOwnerTransform->Get_Look();

	_vector StartQuat = XMQuaternionRotationMatrix(JoMath::LookTo(vPlayerLook));
	_vector EndQuat = XMQuaternionRotationMatrix(JoMath::LookTo(vNewLook));
	
	m_pOwnerTransform->Rotation_Quaternion(XMQuaternionSlerp(StartQuat, EndQuat, m_fRotRate * fTimeDelta));
}

_bool CPlayerState_Base::Check_StateChange(PlayerState eState)
{
	_bool bStateChange = false;

	if (false == m_pPlayer->Can_NextState())
		return false;

	switch (eState)
	{
	case PlayerState::State_Idle:
		bStateChange = KEY_NONE(eKeyCode::W) && KEY_NONE(eKeyCode::A) && KEY_NONE(eKeyCode::S) && KEY_NONE(eKeyCode::D);
		break;
	case PlayerState::State_Jog:
		bStateChange = (KEY_PUSHING(eKeyCode::W) || KEY_PUSHING(eKeyCode::A) || KEY_PUSHING(eKeyCode::S) || KEY_PUSHING(eKeyCode::D));
		break;
	case PlayerState::State_Sprint:
		bStateChange = ((KEY_PUSHING(eKeyCode::W) || KEY_PUSHING(eKeyCode::A) || KEY_PUSHING(eKeyCode::S) || KEY_PUSHING(eKeyCode::D))
			&& KEY_PUSHING(eKeyCode::LShift));
		break;
	case PlayerState::State_LockOn:
		bStateChange = (KEY_PUSHING(eKeyCode::W) || KEY_PUSHING(eKeyCode::A) || KEY_PUSHING(eKeyCode::S) || KEY_PUSHING(eKeyCode::D))
			&& m_pPlayer->Is_LockOn();
		break;
	case PlayerState::State_Attack:
	case PlayerState::State_Execution_Default:
		bStateChange = KEY_PUSHING(eKeyCode::LButton);
		break;

	case PlayerState::State_Avoid:
		bStateChange = KEY_PUSHING(eKeyCode::Space);
		break;
	case PlayerState::State_Parry:
		bStateChange =  KEY_PUSHING(eKeyCode::F);
		break;
	case PlayerState::State_ChargeStart:
	case PlayerState::State_ClawAttack_Short:
		bStateChange = KEY_PUSHING(eKeyCode::RButton);
		break;
	case PlayerState::State_ClawAttack_Long:
	case PlayerState::State_PlunderRush:
		bStateChange = KEY_NONE(eKeyCode::RButton);
		break;
	case PlayerState::State_PlagueAttack:
		bStateChange = KEY_PUSHING(eKeyCode::One) || KEY_PUSHING(eKeyCode::Two);
		break;
	case PlayerState::State_End:
		break;
	}

	return bStateChange;
}

PlayerState CPlayerState_Base::Decide_State()
{
	PlayerState eFinalState = PlayerState::State_End;

	for (PlayerState& State : m_PossibleStates)
	{
		if (Check_StateChange(State))
			eFinalState = State;
	}

	return eFinalState;
}

void CPlayerState_Base::Check_ExtraStateChange(PlayerState eState)
{
	switch (eState)
	{
	case PlayerState::State_PlagueAttack:
		Check_PlagueAttack();
		break;

	default:
		m_pPlayer->Change_State((_uint)eState);
		break;
	}
}

void CPlayerState_Base::Check_PlagueAttack()
{
	//if (KEY_PUSHING(eKeyCode::One))
	//{
	//
	//}

	SKILLTYPE ePlunderSkillType = m_pPlayerStats->Get_PlunderSkillType();

	if (SKILLTYPE::NONE == ePlunderSkillType)
		return;

	PlayerState eState = PlayerState::State_End;

	switch (ePlunderSkillType)
	{
	case SKILLTYPE::AXE:
		eState = PlayerState::State_PW_Axe;
		break;
	case SKILLTYPE::HAMMER:
		eState = PlayerState::State_PW_Hammer;
		break;
	}

	m_pPlayerStats->Update_PlunderSkill(SKILLTYPE::NONE);
	m_pPlayer->Set_NowUsingSkill(ePlunderSkillType);
	m_pPlayer->Change_State((_uint)eState);
}

void CPlayerState_Base::Decide_ClimbState(_int iDir)
{
	if (KEY_PUSHING(eKeyCode::W))
	{
		iDir = (iDir + 1) % 2;
		_uint iNowHeight = m_pPlayer->Add_PlayerLadderHeight(1) + 5;
		if (m_pPlayer->Get_NowLadderHeight() == iNowHeight)
			m_pPlayer->Change_State((_uint)PlayerState::State_Climb_End, &iDir);
		else if (iNowHeight < m_pPlayer->Get_NowLadderHeight())
			m_pPlayer->Change_State((_uint)PlayerState::State_Climb, &iDir);
	}
	else if (KEY_PUSHING(eKeyCode::S))
	{
		iDir = (iDir + 1) % 2;
		_uint iNowHeight = m_pPlayer->Add_PlayerLadderHeight(-1);
		if (iNowHeight <= 1)
			m_pPlayer->Change_State((_uint)PlayerState::State_Climb_End, &iDir);
		else if (iNowHeight > 1)
			m_pPlayer->Change_State((_uint)PlayerState::State_Climb, &iDir);
	}
	else
	{
		if ((_uint)PlayerState::State_Climb_Idle != m_pPlayer->Get_NowState())
			m_pPlayer->Change_State((_uint)PlayerState::State_Climb_Idle, &iDir);
	}
}

void CPlayerState_Base::Free()
{
	__super::Free();

}
