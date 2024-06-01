#include "PlayerState_SprintStart.h"


CPlayerState_SprintStart::CPlayerState_SprintStart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_SprintStart::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

void CPlayerState_SprintStart::OnState_Start()
{
	m_pModel->Reset_RootPos();
	m_pModel->Change_Animation(Corvus_SD1_Sprint_Start);
}

void CPlayerState_SprintStart::OnGoing(_float fTimeDelta)
{
	if (KEY_NONE(eKeyCode::W) && KEY_NONE(eKeyCode::A) && KEY_NONE(eKeyCode::S) && KEY_NONE(eKeyCode::D))
	{
		m_pPlayer->Change_State(PlayerState::State_Idle);
		return;
	}

	_vector vNewLook = Rotate_To_CameraLook(fTimeDelta);
	m_pOwnerTransform->Go_Root(m_pModel->Get_DeltaRootPos(), vNewLook);
	
}

void CPlayerState_SprintStart::OnState_End()
{
}

CPlayerState_SprintStart* CPlayerState_SprintStart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_SprintStart* pInstance = new CPlayerState_SprintStart(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_SprintStart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_SprintStart::Free()
{
	__super::Free();
}
