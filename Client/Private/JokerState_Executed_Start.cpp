#include "JokerState_Executed_Start.h"
#include "Bone.h"


CJokerState_Executed_Start::CJokerState_Executed_Start(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CJokerState_Base(pDevice, pContext)
{
}

HRESULT CJokerState_Executed_Start::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CJokerState_Executed_Start::OnState_Start(void* pArg)
{
	m_AttachDesc = *(ATTACHDESC*)pArg;
	Safe_AddRef(m_AttachDesc.pAttachBone);
	Safe_AddRef(m_AttachDesc.pParentTransform);

	m_pJoker->Set_LookTarget(false);
	m_pJoker->Set_Active_Colliders(false);
	m_pJoker->InActive_StunnedMark();

	PLAY_SOUND(L"Joker_Executed", false, 0.8f);

	m_pModel->Change_Animation(Joker_TakeExecution_Start, 0.f);
}

void CJokerState_Executed_Start::Update(_float fTimeDelta)
{

}

void CJokerState_Executed_Start::Late_Update(_float fTimeDelta)
{
	m_pOwnerTransform->Attach_To_Bone(m_AttachDesc.pAttachBone, m_AttachDesc.pParentTransform, XMMatrixRotationX(-XM_PIDIV2));
}

void CJokerState_Executed_Start::OnState_End()
{
	Safe_Release(m_AttachDesc.pAttachBone);
	Safe_Release(m_AttachDesc.pParentTransform);
}


CJokerState_Executed_Start* CJokerState_Executed_Start::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CJokerState_Executed_Start* pInstance = new CJokerState_Executed_Start(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CJokerState_Executed_Start"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJokerState_Executed_Start::Free()
{
	__super::Free();
}
