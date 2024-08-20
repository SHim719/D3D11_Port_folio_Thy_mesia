#include "VillagerFState_Attack3.h"

CVillagerFState_Attack3::CVillagerFState_Attack3(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVillagerFState_Base(pDevice, pContext)
{
}

HRESULT CVillagerFState_Attack3::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModel->Bind_Func("Sound_StrongAttackVoice", bind(&CGameInstance::Play, m_pGameInstance, L"VillagerF_Voice_AttackLong", false, 0.5f));

	return S_OK;
}

void CVillagerFState_Attack3::OnState_Start(void* pArg)
{
	Reset_AttackIdx();
	m_pVillager_F->Set_Stanced(true);
	m_pVillager_F->Set_LookTarget(true);
	m_pVillager_F->Update_AttackDesc();
	m_pModel->Change_Animation(LV1Villager_F_Attack04);
}

void CVillagerFState_Attack3::Update(_float fTimeDelta)
{
	if (!m_pVillager_F->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}
 
void CVillagerFState_Attack3::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_State();
}

void CVillagerFState_Attack3::OnState_End()
{

}

void CVillagerFState_Attack3::Init_AttackDesc()
{
	m_AttackDescs.reserve(1);

	ATTACKDESC AttackDesc;
	AttackDesc.iDamage = 40;
	AttackDesc.eEnemyAttackType = NORMAL;
	AttackDesc.pAttacker = m_pVillager_F;

	m_AttackDescs.emplace_back(CVillager_F::HAND, AttackDesc);
}

CVillagerFState_Attack3* CVillagerFState_Attack3::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CVillagerFState_Attack3* pInstance = new CVillagerFState_Attack3(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CVillagerFState_Attack3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillagerFState_Attack3::Free()
{
	__super::Free();
}
