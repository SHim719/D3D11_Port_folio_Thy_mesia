#include "TwinBladeKnightState_Executed_Start.h"
#include "Bone.h"


CTwinBladeKnightState_Executed_Start::CTwinBladeKnightState_Executed_Start(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTwinBladeKnightState_Base(pDevice, pContext)
{
}

HRESULT CTwinBladeKnightState_Executed_Start::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTwinBladeKnightState_Executed_Start::OnState_Start(void* pArg)
{
	m_AttachDesc = *(ATTACHDESC*)pArg;
	Safe_AddRef(m_AttachDesc.pAttachBone);
	Safe_AddRef(m_AttachDesc.pParentTransform);

	m_pTwinBladeKnight->Set_LookTarget(false);
	m_pTwinBladeKnight->Set_Active_Colliders(false);
	m_pTwinBladeKnight->InActive_StunnedMark();

	m_pModel->Change_Animation(HArmorLV1_Halberds_VS_TakeExecution, 0.f, false);
}

void CTwinBladeKnightState_Executed_Start::Update(_float fTimeDelta)
{

}

void CTwinBladeKnightState_Executed_Start::Late_Update(_float fTimeDelta)
{
	m_pOwnerTransform->Attach_To_Bone(m_AttachDesc.pAttachBone, m_AttachDesc.pParentTransform, XMMatrixRotationX(-XM_PIDIV2));
}

void CTwinBladeKnightState_Executed_Start::OnState_End()
{
	Safe_Release(m_AttachDesc.pAttachBone);
	Safe_Release(m_AttachDesc.pParentTransform);
}


CTwinBladeKnightState_Executed_Start* CTwinBladeKnightState_Executed_Start::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTwinBladeKnightState_Executed_Start* pInstance = new CTwinBladeKnightState_Executed_Start(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTwinBladeKnightState_Executed_Start"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTwinBladeKnightState_Executed_Start::Free()
{
	__super::Free();
}
