#include "PlayerState_Idle.h"


CPlayerState_Idle::CPlayerState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Idle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayerState_Idle::OnState_Start()
{
	m_pModel->Change_Animation(Corvus_SD_Idle);
}

void CPlayerState_Idle::OnGoing(_float fTimeDelta)
{
	if (KEY_DOWN(eKeyCode::W) || KEY_DOWN(eKeyCode::A) || KEY_DOWN(eKeyCode::S) || KEY_DOWN(eKeyCode::D))
	{
		if (KEY_PUSHING(eKeyCode::LShift))
			m_pPlayer->Change_State(PlayerState::State_SprintStart);
		else
			m_pPlayer->Change_State(PlayerState::State_Jog);
		
	}
}

void CPlayerState_Idle::OnState_End()
{
}

CPlayerState_Idle* CPlayerState_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Idle* pInstance = new CPlayerState_Idle(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Idle::Free()
{
	__super::Free();
}
