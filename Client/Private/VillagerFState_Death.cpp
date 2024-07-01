#include "VillagerFState_Death.h"

#include "EnemyStats.h"

CVillagerFState_Death::CVillagerFState_Death(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVillagerFState_Base(pDevice, pContext)
{
}

HRESULT CVillagerFState_Death::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVillagerFState_Death::OnState_Start(void* pArg)
{
	m_pVillager_F->Set_LookTarget(false);
	m_pVillager_F->Set_Active_Colliders(false);
	m_pVillager_F->InActive_StunnedMark();

	m_pVillager_F->Get_Stats()->Broadcast_Death();

	m_pModel->Change_Animation(LV1Villager_F_Die01, 0.1f, false);
}

void CVillagerFState_Death::Update(_float fTimeDelta)
{
	if (false == m_pModel->Is_AnimComplete())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CVillagerFState_Death::Late_Update(_float fTimeDelta)
{

}

void CVillagerFState_Death::OnState_End()
{
}

void CVillagerFState_Death::OnHit(const ATTACKDESC& AttackDesc)
{

}

CVillagerFState_Death* CVillagerFState_Death::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CVillagerFState_Death* pInstance = new CVillagerFState_Death(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CVillagerFState_Death"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillagerFState_Death::Free()
{
	__super::Free();
}
