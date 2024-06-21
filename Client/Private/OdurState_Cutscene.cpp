#include "OdurState_Cutscene.h"

COdurState_Cutscene::COdurState_Cutscene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_Cutscene::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void COdurState_Cutscene::OnState_Start(void* pArg)
{
	m_pOdur->Disable_LookTarget();
	m_pOdur->Enable_Stanced();
	m_pOdur->Reset_AttackIdx();


	ATTACHDESC attachDesc;
	attachDesc.pAttachBone = m_pModel->Get_Bone("AnimTargetPoint");
	attachDesc.pParentTransform = m_pOwnerTransform;

	m_pCard = m_pGameInstance->Add_Clone(m_pGameInstance->Get_CurrentLevelID(), L"Object", L"Prototype_Odut_Cutscene_Card", &attachDesc);

	m_pModel->Change_Animation(MagicianLV1_Seq_BossFightStart, 0.f);
}

void COdurState_Cutscene::OnGoing(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		_float fWalkTime = 4.f;
		m_pOdur->Change_State((_uint)OdurState::State_Walk, &fWalkTime);
	}
		
}

void COdurState_Cutscene::OnState_End()
{

}

COdurState_Cutscene* COdurState_Cutscene::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_Cutscene* pInstance = new COdurState_Cutscene(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_Cutscene"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_Cutscene::Free()
{
	__super::Free();
}
