#include "VillagerMState_Stunned_Loop.h"

#include "Player.h"

CVillagerMState_Stunned_Loop::CVillagerMState_Stunned_Loop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVillagerMState_Base(pDevice, pContext)
{
}

HRESULT CVillagerMState_Stunned_Loop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVillagerMState_Stunned_Loop::OnState_Start(void* pArg)
{
	static_cast<CPlayer*>(m_pVillager_M->Get_Target())->Add_StunnedEnemy(m_pVillager_M);

	m_pModel->Change_Animation(LV1Villager_M_HurtStunLoop);
	
}

void CVillagerMState_Stunned_Loop::Update(_float fTimeDelta)
{

}

void CVillagerMState_Stunned_Loop::Late_Update(_float fTimeDelta)
{
}

void CVillagerMState_Stunned_Loop::OnState_End()
{
}

void CVillagerMState_Stunned_Loop::OnHit(const ATTACKDESC& AttackDesc)
{

}

CVillagerMState_Stunned_Loop* CVillagerMState_Stunned_Loop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CVillagerMState_Stunned_Loop* pInstance = new CVillagerMState_Stunned_Loop(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CVillagerMState_Stunned_Loop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillagerMState_Stunned_Loop::Free()
{
	__super::Free();
}
