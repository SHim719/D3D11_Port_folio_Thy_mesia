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

	CTransform* pCameraTransform = m_pMain_Camera->Get_Transform();
	_vector vGroundLook = pCameraTransform->Get_GroundLook();
	_vector vPlayerLook = m_pOwnerTransform->Get_Look();

	if (KEY_PUSHING(eKeyCode::W))
	{
		_vector vYAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vQuat = (XMQuaternionSlerp(m_pOwnerTransform->Calculate_VecToQuat(vYAxis, vPlayerLook), m_pOwnerTransform->Calculate_VecToQuat(vYAxis, vGroundLook), 0.2f));
		m_pOwnerTransform->Rotation_Quaternion(vQuat);
	}

	if (KEY_DOWN(eKeyCode::W) || KEY_DOWN(eKeyCode::A) || KEY_DOWN(eKeyCode::S) || KEY_DOWN(eKeyCode::D))
	{
		if (KEY_PUSHING(eKeyCode::LShift))
		{
			
		}
		else
		{
			
		}
	}

	m_pOwnerTransform->Go_Straight(fTimeDelta);
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
