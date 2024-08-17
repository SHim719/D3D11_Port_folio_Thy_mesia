#include "UrdState_Executed.h"
#include "Bone.h"

#include "UI_Manager.h"


CUrdState_Executed::CUrdState_Executed(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_Executed::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUrdState_Executed::OnState_Start(void* pArg)
{
	m_AttachDesc = *(ATTACHDESC*)pArg;
	Safe_AddRef(m_AttachDesc.pAttachBone);
	Safe_AddRef(m_AttachDesc.pParentTransform);

	m_pUrd->Set_LookTarget(false);
	m_pUrd->Set_Active_Colliders(false);
	m_pUrd->InActive_StunnedMark();

	//m_pGameInstance->Set_TimeScaleWithRealTime(0.2f, 1.5f);

	m_pModel->Change_Animation(Urd_VS_TakeExecution, 0.f, false);
}

void CUrdState_Executed::Update(_float fTimeDelta)
{

}

void CUrdState_Executed::Late_Update(_float fTimeDelta)
{
	if (false == m_pUrd->Is_ExecutionEnd())
		m_pOwnerTransform->Attach_To_Bone(m_AttachDesc.pAttachBone, m_AttachDesc.pParentTransform, XMMatrixRotationX(-XM_PIDIV2));

	//if (true == m_pModel->Is_AnimComplete() && false == m_pUrd->Is_Dissolving())
	//	m_pUrd->Active_Dissolve();

}

void CUrdState_Executed::OnState_End()
{
	Safe_Release(m_AttachDesc.pAttachBone);
	Safe_Release(m_AttachDesc.pParentTransform);
}


CUrdState_Executed* CUrdState_Executed::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_Executed* pInstance = new CUrdState_Executed(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_Executed"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_Executed::Free()
{
	__super::Free();

	Safe_Release(m_AttachDesc.pAttachBone);
	Safe_Release(m_AttachDesc.pParentTransform);
}
