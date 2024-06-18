#include "OdurState_ReadyExecution.h"

#include "Player.h"
#include "Bone.h"
#include "Main_Camera.h"

COdurState_ReadyExecution::COdurState_ReadyExecution(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_ReadyExecution::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void COdurState_ReadyExecution::OnState_Start(void* pArg)
{
	m_pOdur->Enable_LookTarget();
	m_pOdur->Enable_Stanced();
	m_pOdur->Reset_AttackIdx();

	m_pModel->Change_Animation(Magician_SPAttack1, 0.f);
}

void COdurState_ReadyExecution::OnGoing(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		ChangeState_Execution();
		return;
	}

	if (false == m_pOdur->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos());
	else
		ChangeState_Execution();	

}

void COdurState_ReadyExecution::OnState_End()
{

}

void COdurState_ReadyExecution::ChangeState_Execution()
{
	EXECUTEDDESC ExecutedDesc;

	ExecutedDesc.AttachDesc.pAttachBone = m_pModel->Get_Bone("AnimTargetPoint");
	ExecutedDesc.AttachDesc.pParentTransform = m_pOwnerTransform;
	ExecutedDesc.iExecutedAnimIndex = Corvus_VSMagician_SP1Hurt;

	static_cast<CPlayer*>(m_pOdur->Get_Target())->SetState_Executed(&ExecutedDesc);

	m_pOdur->Change_State((_uint)OdurState::State_Execute);
}


COdurState_ReadyExecution* COdurState_ReadyExecution::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_ReadyExecution* pInstance = new COdurState_ReadyExecution(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_ReadyExecution"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_ReadyExecution::Free()
{
	__super::Free();
}