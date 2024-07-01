#include "VillagerMState_Idle.h"

CVillagerMState_Idle::CVillagerMState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVillagerMState_Base(pDevice, pContext)
{
}

HRESULT CVillagerMState_Idle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVillagerMState_Idle::OnState_Start(void* pArg)
{
	m_pModel->Change_Animation(LV1Villager_M_IdleGeneral);
}

void CVillagerMState_Idle::Update(_float fTimeDelta)
{

}

void CVillagerMState_Idle::OnState_End()
{

}

CVillagerMState_Idle* CVillagerMState_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CVillagerMState_Idle* pInstance = new CVillagerMState_Idle(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CVillagerMState_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillagerMState_Idle::Free()
{
	__super::Free();
}
