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

void CPlayerState_Base::OnState_Start()
{
}

void CPlayerState_Base::OnGoing(_float fTimeDelta)
{
}

void CPlayerState_Base::OnState_End()
{
}

_vector CPlayerState_Base::Calc_NewLook()
{
	CTransform* pCameraTransform = m_pMain_Camera->Get_Transform();
	_vector vCameraLook = pCameraTransform->Get_GroundLook();
	_vector vCameraRight = pCameraTransform->Get_GroundRight();
	
	_vector vNewLook = XMVectorZero();

	if (KEY_PUSHING(eKeyCode::W))
		vNewLook += vCameraLook;
	if (KEY_PUSHING(eKeyCode::S))
		vNewLook -= vCameraLook;
	if (KEY_PUSHING(eKeyCode::D))
		vNewLook += vCameraRight;
	if (KEY_PUSHING(eKeyCode::A))
		vNewLook -= vCameraRight;

	if (XMVectorGetX(XMVector3Length(vNewLook)) < FLT_EPSILON)
		return XMVectorZero();

	return XMVector3Normalize(vNewLook);
}

void CPlayerState_Base::Rotate_To_Look(_vector vNewLook, _float fTimeDelta)
{
	_vector vPlayerLook = m_pOwnerTransform->Get_Look();

	_vector StartQuat = XMQuaternionRotationMatrix(Get_LookTo(vPlayerLook));
	_vector EndQuat = XMQuaternionRotationMatrix(Get_LookTo(vNewLook));

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
	case PlayerState::State_Sprint:
		bStateChange = (KEY_PUSHING(eKeyCode::W) || KEY_PUSHING(eKeyCode::A) || KEY_PUSHING(eKeyCode::S) || KEY_PUSHING(eKeyCode::D)) 
			&& KEY_PUSHING(eKeyCode::LShift);
		break;
	case PlayerState::State_Attack:
		bStateChange = KEY_PUSHING(eKeyCode::LButton);
		break;
	case PlayerState::State_End:
		break;
	}

	return bStateChange;
}

void CPlayerState_Base::Free()
{
	__super::Free();

}
