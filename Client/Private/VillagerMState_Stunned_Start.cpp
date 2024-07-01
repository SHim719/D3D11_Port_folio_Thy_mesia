#include "VillagerMState_Stunned_Start.h"


CVillagerMState_Stunned_Start::CVillagerMState_Stunned_Start(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVillagerMState_Base(pDevice, pContext)
{
}

HRESULT CVillagerMState_Stunned_Start::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVillagerMState_Stunned_Start::OnState_Start(void* pArg)
{
	m_pVillager_M->Set_LookTarget(false);

	m_pModel->Change_Animation(LV1Villager_M_HurtStunStart);
}

void CVillagerMState_Stunned_Start::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CVillagerMState_Stunned_Start::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pVillager_M->Change_State((_uint)VillagerM_State::State_Stunned_Loop);
		
}

void CVillagerMState_Stunned_Start::OnState_End()
{
}

void CVillagerMState_Stunned_Start::OnHit(const ATTACKDESC& AttackDesc)
{
	
}

CVillagerMState_Stunned_Start* CVillagerMState_Stunned_Start::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CVillagerMState_Stunned_Start* pInstance = new CVillagerMState_Stunned_Start(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CVillagerMState_Stunned_Start"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillagerMState_Stunned_Start::Free()
{
	__super::Free();
}
