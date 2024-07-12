#include "PlayerState_Climb_Idle.h"

CPlayerState_Climb_Idle::CPlayerState_Climb_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Climb_Idle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayerState_Climb_Idle::OnState_Start(void* pArg)
{
	m_iDir = *(_int*)pArg;

	m_pModel->Change_Animation(Corvus_SD_Ladder_L_Idle + m_iDir);
}

void CPlayerState_Climb_Idle::Update(_float fTimeDelta)
{

}

void CPlayerState_Climb_Idle::Late_Update(_float fTimeDelta)
{
	Decide_ClimbState(m_iDir);
}

void CPlayerState_Climb_Idle::OnState_End()
{

}

CPlayerState_Climb_Idle* CPlayerState_Climb_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Climb_Idle* pInstance = new CPlayerState_Climb_Idle(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Climb_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Climb_Idle::Free()
{
	__super::Free();
}
