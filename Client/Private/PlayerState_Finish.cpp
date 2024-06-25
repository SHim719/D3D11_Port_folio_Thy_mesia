#include "PlayerState_Finish.h"

CPlayerState_Finish::CPlayerState_Finish(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Finish::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayerState_Finish::OnState_Start(void* pArg)
{
	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_CanRotation(false);
	m_pPlayer->Inactive_AllWeaponColliders();

	m_pModel->Change_Animation(Corvus_SD_Rebound_R, 0.f);
}

void CPlayerState_Finish::OnGoing(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
		return;
	}

	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}


void CPlayerState_Finish::OnState_End()
{

}

CPlayerState_Finish* CPlayerState_Finish::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Finish* pInstance = new CPlayerState_Finish(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Finish"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Finish::Free()
{
	__super::Free();
}
