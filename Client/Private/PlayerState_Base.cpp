#include "PlayerState_Base.h"


CPlayerState_Base::CPlayerState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Base::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pPlayer = (CPlayer*)pArg;

	m_pMain_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Find_GameObject(GET_CURLEVEL, L"Camera", 0));
	if (nullptr == m_pMain_Camera)
		return E_FAIL;

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	return S_OK;
}

void CPlayerState_Base::OnState_Start(void* pArg)
{
}

void CPlayerState_Base::OnGoing(_float fTimeDelta)
{
}

void CPlayerState_Base::OnState_End()
{
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

	switch (eState)
	{
	case PlayerState::State_Idle:
		bStateChange = KEY_NONE(eKeyCode::W) && KEY_NONE(eKeyCode::A) && KEY_NONE(eKeyCode::S) && KEY_NONE(eKeyCode::D);
		break;
	case PlayerState::State_Jog:
		bStateChange = KEY_PUSHING(eKeyCode::W) || KEY_PUSHING(eKeyCode::A) || KEY_PUSHING(eKeyCode::S) || KEY_PUSHING(eKeyCode::D);
		break;
	case PlayerState::State_LockOn:
		bStateChange = (KEY_PUSHING(eKeyCode::W) || KEY_PUSHING(eKeyCode::A) || KEY_PUSHING(eKeyCode::S) || KEY_PUSHING(eKeyCode::D))
			&& m_pPlayer->Is_LockOn();
		break;
	case PlayerState::State_Sprint:
		bStateChange = (KEY_PUSHING(eKeyCode::W) || KEY_PUSHING(eKeyCode::A) || KEY_PUSHING(eKeyCode::S) || KEY_PUSHING(eKeyCode::D)) 
			&& KEY_PUSHING(eKeyCode::LShift);
		break;
	case PlayerState::State_Attack:
		bStateChange = m_pPlayer->Can_NextState() && KEY_PUSHING(eKeyCode::LButton);
		break;
	case PlayerState::State_Avoid:
		bStateChange = m_pPlayer->Can_NextState() && KEY_PUSHING(eKeyCode::Space);
		break;
	case PlayerState::State_Parry:
		bStateChange = m_pPlayer->Can_NextState() && KEY_PUSHING(eKeyCode::F);
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

void CPlayerState_Base::Free()
{
	__super::Free();

}
