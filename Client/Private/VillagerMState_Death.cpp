#include "VillagerMState_Death.h"

#include "EnemyStats.h"

#include "Main_Camera.h"

CVillagerMState_Death::CVillagerMState_Death(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVillagerMState_Base(pDevice, pContext)
{
}

HRESULT CVillagerMState_Death::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVillagerMState_Death::OnState_Start(void* pArg)
{
	m_pOwnerTransform->LookAt2D(m_pTargetTransform->Get_Position());
	m_pVillager_M->Set_LookTarget(false);
	m_pVillager_M->Set_Active_Colliders(false);
	m_pVillager_M->InActive_StunnedMark();

	static_cast<CMain_Camera*>(GET_CAMERA)->Play_CameraShake("Shaking_Execution");

	m_pGameInstance->Play(L"VillagerM_Voice_Die", false, 1.f);

	m_pModel->Change_Animation(LV1Villager_M_Die01, 0.1f, false); 
}

void CVillagerMState_Death::Update(_float fTimeDelta)
{
	if (!m_pModel->Is_AnimComplete())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
	
		
}

void CVillagerMState_Death::Late_Update(_float fTimeDelta)
{

}

void CVillagerMState_Death::OnState_End()
{
}

void CVillagerMState_Death::OnHit(const ATTACKDESC& AttackDesc)
{

}

CVillagerMState_Death* CVillagerMState_Death::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CVillagerMState_Death* pInstance = new CVillagerMState_Death(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CVillagerMState_Death"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillagerMState_Death::Free()
{
	__super::Free();
}

