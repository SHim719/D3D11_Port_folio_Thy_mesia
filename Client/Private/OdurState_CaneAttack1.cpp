#include "OdurState_CaneAttack1.h"

COdurState_CaneAttack1::COdurState_CaneAttack1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_CaneAttack1::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void COdurState_CaneAttack1::OnState_Start(void* pArg)
{
	m_pOdur->Set_Stanced(true);
	m_pOdur->Set_LookTarget(true);
	Reset_AttackIdx();

	for (_uint i = 0; i < 2; ++i)
		m_pOdur->Update_AttackDesc();
	
	m_pModel->Change_Animation(Magician_CaneAttack03);

	EFFECTMGR->Active_Effect("Effect_Odur_Cane_Attack1_Cloak_Particle", &m_pOdur->Get_EffectSpawnDesc());
}

void COdurState_CaneAttack1::Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pOdur->Change_State((_uint)OdurState::State_Walk);

	if (false == m_pOdur->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void COdurState_CaneAttack1::OnState_End()
{

}

void COdurState_CaneAttack1::Init_AttackDesc()
{
	m_AttackDescs.reserve(2);

	ATTACKDESC AttackDesc;
	AttackDesc.pAttacker = m_pOdur;
	AttackDesc.eEnemyAttackType = NORMAL;
	AttackDesc.iDamage = 48;

	m_AttackDescs.emplace_back(COdur::CANE, AttackDesc);

	AttackDesc.eEnemyAttackType = KNOCKBACK;
	AttackDesc.iDamage = 65;

	m_AttackDescs.emplace_back(COdur::FOOT_L, AttackDesc);
}

COdurState_CaneAttack1* COdurState_CaneAttack1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_CaneAttack1* pInstance = new COdurState_CaneAttack1(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_CaneAttack1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_CaneAttack1::Free()
{
	__super::Free();
}
