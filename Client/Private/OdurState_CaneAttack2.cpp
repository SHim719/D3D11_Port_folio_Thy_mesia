#include "OdurState_CaneAttack2.h"

COdurState_CaneAttack2::COdurState_CaneAttack2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_CaneAttack2::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModel->Bind_Func("ChangeAnim_Cane2ToCane1", bind(&COdurState_CaneAttack2::ChangeAnim_Cane2ToCane1, this));

	return S_OK;
}

void COdurState_CaneAttack2::OnState_Start(void* pArg)
{
	m_pOdur->Set_Stanced(true);
	m_pOdur->Set_LookTarget(true);
	Reset_AttackIdx();

	m_pOdur->Update_AttackDesc();

	m_pModel->Change_Animation(Magician_CaneAttack2);
}

void COdurState_CaneAttack2::Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		Decide_State();
		return;
	}
		
	if (false == m_pOdur->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void COdurState_CaneAttack2::OnState_End()
{

}

void COdurState_CaneAttack2::Init_AttackDesc()
{
	m_AttackDescs.reserve(3);

	ATTACKDESC AttackDesc;
	AttackDesc.pAttacker = m_pOdur;
	AttackDesc.eEnemyAttackType = NORMAL;

	m_AttackDescs.emplace_back(COdur::CANE, AttackDesc);
	m_AttackDescs.emplace_back(COdur::CANE, AttackDesc);

	AttackDesc.eEnemyAttackType = KNOCKBACK;

	m_AttackDescs.emplace_back(COdur::CANE, AttackDesc);

}

void COdurState_CaneAttack2::ChangeAnim_Cane2ToCane1()
{
	ADD_EVENT(bind(&CModel::Change_AnimationWithStartFrame, m_pModel, Magician_CaneAttack1Appear, 25, 0.1f, true));

}

COdurState_CaneAttack2* COdurState_CaneAttack2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_CaneAttack2* pInstance = new COdurState_CaneAttack2(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_CaneAttack2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_CaneAttack2::Free()
{
	__super::Free();
}
