#include "OdurState_KickCombo.h"

COdurState_KickCombo::COdurState_KickCombo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_KickCombo::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void COdurState_KickCombo::OnState_Start(void* pArg)
{
	m_pOdur->Reset_AttackIdx();
	m_pOdur->Enable_LookTarget();
	m_pModel->Change_Animation(Magician_KickCombo);
}

void COdurState_KickCombo::OnGoing(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		m_pOdur->Swap_Bone();
		//m_pOdur->Change_State((_uint)OdurState::State_DisappearMove);
		return;
	}

	if (false == m_pOdur->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos());
}

void COdurState_KickCombo::OnState_End()
{

}

void COdurState_KickCombo::Init_AttackDesc()
{
	m_AttackDescs.resize(3);

	ATTACKDESC AttackDesc;
	AttackDesc.eAttackType = NORMAL;

	m_AttackDescs[0] = AttackDesc;
	m_AttackDescs[1] = AttackDesc;

	AttackDesc.eAttackType = KNOCKBACK;

	m_AttackDescs[2] = AttackDesc;
}

COdurState_KickCombo* COdurState_KickCombo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_KickCombo* pInstance = new COdurState_KickCombo(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_KickCombo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_KickCombo::Free()
{
	__super::Free();
}
