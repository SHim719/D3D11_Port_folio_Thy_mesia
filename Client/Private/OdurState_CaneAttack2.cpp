#include "OdurState_CaneAttack2.h"

COdurState_CaneAttack2::COdurState_CaneAttack2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_CaneAttack2::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void COdurState_CaneAttack2::OnState_Start(void* pArg)
{
	m_pOdur->Reset_AttackIdx();
	m_pOdur->Enable_Stanced();
	m_pModel->Change_Animation(Magician_CaneAttack1Appear);
}

void COdurState_CaneAttack2::OnGoing(_float fTimeDelta)
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
	m_AttackDescs.resize(2);

	ATTACKDESC AttackDesc;
	AttackDesc.eAttackType = NORMAL;

	m_AttackDescs[0] = AttackDesc;

	AttackDesc.eAttackType = KNOCKBACK;

	m_AttackDescs[1] = AttackDesc;
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
