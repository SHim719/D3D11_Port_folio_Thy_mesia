#include "VillagerFState_Attack2.h"

CVillagerFState_Attack2::CVillagerFState_Attack2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVillagerFState_Base(pDevice, pContext)
{
}

HRESULT CVillagerFState_Attack2::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModel->Bind_Func("ChangeToNextAttackAnim", bind(&CVillagerFState_Attack2::Change_To_NextAttackAnim, this));

	return S_OK;
}

void CVillagerFState_Attack2::OnState_Start(void* pArg)
{
	Reset_AttackIdx();
	m_pVillager_F->Set_Stanced(true);
	m_pVillager_F->Set_LookTarget(true);
	m_pVillager_F->Update_AttackDesc();
	m_pModel->Change_Animation(LV1Villager_F_Attack02);
}

void CVillagerFState_Attack2::Update(_float fTimeDelta)
{
	if (!m_pVillager_F->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CVillagerFState_Attack2::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_State();
}

void CVillagerFState_Attack2::OnState_End()
{

}

void CVillagerFState_Attack2::Init_AttackDesc()
{
	m_AttackDescs.reserve(2);

	ATTACKDESC AttackDesc;
	AttackDesc.iDamage = 40;
	AttackDesc.eEnemyAttackType = NORMAL;
	AttackDesc.pAttacker = m_pVillager_F;

	m_AttackDescs.emplace_back(CVillager_F::HAND, AttackDesc);
	m_AttackDescs.emplace_back(CVillager_F::HAND, AttackDesc);
}

void CVillagerFState_Attack2::Change_To_NextAttackAnim()
{
	if (!m_pVillager_F->Is_CollPlayer())
		ADD_EVENT(bind(&CModel::Change_Animation, m_pModel, LV1Villager_F_Attack03, 0.1f, true));
}

CVillagerFState_Attack2* CVillagerFState_Attack2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CVillagerFState_Attack2* pInstance = new CVillagerFState_Attack2(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CVillagerFState_Attack2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillagerFState_Attack2::Free()
{
	__super::Free();
}
