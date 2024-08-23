#include "OdurState_Executed.h"
#include "Bone.h"

#include "UI_Manager.h"


COdurState_Executed::COdurState_Executed(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_Executed::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void COdurState_Executed::OnState_Start(void* pArg)
{
	m_AttachDesc = *(ATTACHDESC*)pArg;
	Safe_AddRef(m_AttachDesc.pAttachBone);
	Safe_AddRef(m_AttachDesc.pParentTransform);

	m_pOdur->Set_LookTarget(false);
	m_pOdur->Set_Active_Colliders(false);
	m_pOdur->InActive_StunnedMark();

	m_pGameInstance->Set_TimeScaleWithRealTime(0.2f, 1.1f);

	PLAY_SOUND(L"Odur_Executed_All", false, 1.f);

	m_pModel->Change_Animation(Magician_VSCorvus_TakeExecution, 0.f, false);
}

void COdurState_Executed::Update(_float fTimeDelta)
{
	if (false == m_pOdur->Is_ExecutionEnd())
		m_pOwnerTransform->Attach_To_Bone(m_AttachDesc.pAttachBone, m_AttachDesc.pParentTransform, XMMatrixRotationX(-XM_PIDIV2));
}

void COdurState_Executed::Late_Update(_float fTimeDelta)
{
	if (true == m_pModel->Is_AnimComplete() && false == m_pOdur->Is_Dissolving())
	{
		m_pOdur->Active_Dissolve();
		m_pGameInstance->Set_Sound_FadeOut(L"BGM_Odur", 1.f);
	}
		

}

void COdurState_Executed::OnState_End()
{
	Safe_Release(m_AttachDesc.pAttachBone);
	Safe_Release(m_AttachDesc.pParentTransform);
}



COdurState_Executed* COdurState_Executed::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_Executed* pInstance = new COdurState_Executed(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_Executed"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_Executed::Free()
{
	__super::Free();

	Safe_Release(m_AttachDesc.pAttachBone);
	Safe_Release(m_AttachDesc.pParentTransform);
}
