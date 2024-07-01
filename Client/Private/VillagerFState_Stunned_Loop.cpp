#include "VillagerFState_Stunned_Loop.h"

#include "Player.h"

CVillagerFState_Stunned_Loop::CVillagerFState_Stunned_Loop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVillagerFState_Base(pDevice, pContext)
{
}

HRESULT CVillagerFState_Stunned_Loop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVillagerFState_Stunned_Loop::OnState_Start(void* pArg)
{
	m_pModel->Change_Animation(LV1Villager_F_HurtStunLoop);
	static_cast<CPlayer*>(m_pVillager_F->Get_Target())->Add_StunnedEnemy(m_pVillager_F);
}

void CVillagerFState_Stunned_Loop::Update(_float fTimeDelta)
{

}

void CVillagerFState_Stunned_Loop::Late_Update(_float fTimeDelta)
{
}

void CVillagerFState_Stunned_Loop::OnState_End()
{
}

void CVillagerFState_Stunned_Loop::OnHit(const ATTACKDESC& AttackDesc)
{

}

CVillagerFState_Stunned_Loop* CVillagerFState_Stunned_Loop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CVillagerFState_Stunned_Loop* pInstance = new CVillagerFState_Stunned_Loop(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CVillagerFState_Stunned_Loop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillagerFState_Stunned_Loop::Free()
{
	__super::Free();
}
