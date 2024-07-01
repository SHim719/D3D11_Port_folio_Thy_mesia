#include "VillagerFState_Idle.h"

CVillagerFState_Idle::CVillagerFState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVillagerFState_Base(pDevice, pContext)
{
}

HRESULT CVillagerFState_Idle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVillagerFState_Idle::OnState_Start(void* pArg)
{
	m_pModel->Change_Animation(LV1Villager_F_Idle);
}

void CVillagerFState_Idle::Update(_float fTimeDelta)
{

}

void CVillagerFState_Idle::OnState_End()
{

}

CVillagerFState_Idle* CVillagerFState_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CVillagerFState_Idle* pInstance = new CVillagerFState_Idle(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CVillagerFState_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillagerFState_Idle::Free()
{
	__super::Free();
}
