#include "PlayerState_Jog.h"


CPlayerState_Jog::CPlayerState_Jog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Jog::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayerState_Jog::OnState_Start()
{
	m_pModel->Change_Animation(Corvus_SD_RunF_24);
}

void CPlayerState_Jog::OnGoing(_float fTimeDelta)
{
	if (KEY_NONE(eKeyCode::W) && KEY_NONE(eKeyCode::A) && KEY_NONE(eKeyCode::S) && KEY_NONE(eKeyCode::D))
	{
		m_pPlayer->Change_State(PlayerState::State_Idle);
		return;
	}

	if (KEY_PUSHING(eKeyCode::LShift))
	{

	}

	//_float vDot = XMVectorGetX(XMVector3Dot(vPlayerLook, vNewLook));

	//if (vDot > 0.99f)
	//{
	//	
	//}

	CTransform* pCameraTransform = m_pMain_Camera->Get_Transform();
	_vector vCameraLook = pCameraTransform->Get_GroundLook();
	_vector vCameraRight = pCameraTransform->Get_GroundRight();
	_vector vPlayerLook = m_pOwnerTransform->Get_Look();
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
		return;

	vNewLook = XMVector3Normalize(vNewLook);

	_vector StartQuat = XMQuaternionRotationMatrix(Get_LookTo(vPlayerLook));
	_vector EndQuat = XMQuaternionRotationMatrix(Get_LookTo(vNewLook));

	m_pOwnerTransform->Rotation_Quaternion(XMQuaternionSlerp(StartQuat, EndQuat, m_fRotRate * fTimeDelta));

	m_pOwnerTransform->Go_Dir(vNewLook, fTimeDelta);	
}

void CPlayerState_Jog::OnState_End()
{
}

CPlayerState_Jog* CPlayerState_Jog::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Jog* pInstance = new CPlayerState_Jog(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Jog"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Jog::Free()
{
	__super::Free();
}
