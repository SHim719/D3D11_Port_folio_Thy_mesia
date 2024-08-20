#include "UrdState_Executed.h"
#include "Bone.h"

#include "UI_Manager.h"

#include "BossMusic_Player.h"


CUrdState_Executed::CUrdState_Executed(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_Executed::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModel->Bind_Func("Only_Position", bind(&CUrdState_Executed::Set_OnlyPosition, this));
	m_pModel->Bind_Func("Effect_Urd_ExecutionBlood", bind(&CUrdState_Executed::Spawn_Effect_Execution_Blood_Phase2, this));
	m_pModel->Bind_Func("Urd_Dead", bind(&CUrdState_Executed::Die, this));

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
	m_pUrd->Set_ExecutionEnd(false);
	m_bOnlyPosition = false;

	//m_pTargetTransform->Set_Position(m_pUrd->Get_InitWorldMatrix().r[3]);

	m_pOwnerTransform->LookAt2D(m_pTargetTransform->Get_Position());
	m_pOwnerTransform->Add_YAxisInput(XM_PI);

	PLAY_SOUND(L"Urd_Cutscene_Execution", false, 0.5f);

	m_pModel->Change_Animation(Urd_VS_TakeExecution, 0.f, false);

	if (false == m_pUrd->Is_Phase2())
		Get_Inst(CBossMusic_Player)->Play_NextBGM();
}

void CUrdState_Executed::Update(_float fTimeDelta)
{
	if (false == m_pUrd->Is_ExecutionEnd())
	{
		m_pOwnerTransform->Attach_To_Bone(m_AttachDesc.pAttachBone, m_AttachDesc.pParentTransform, XMMatrixRotationX(-XM_PIDIV2) *
			XMMatrixRotationZ(XM_PI), m_bOnlyPosition);
	}
	

}

void CUrdState_Executed::Late_Update(_float fTimeDelta)
{
	if (true == m_pModel->Is_AnimComplete())
	{
		if (m_pUrd->Is_Phase2())
		{
			m_pUrd->Change_State((_uint)UrdState::State_Dead);
		}
		else
		{
			m_pUrd->Change_State((_uint)UrdState::State_Ultimate);
			m_pUrd->Active_Phase2();
		}
	}


}

void CUrdState_Executed::OnState_End()
{
	Safe_Release(m_AttachDesc.pAttachBone);
	Safe_Release(m_AttachDesc.pParentTransform);

	Setup_RootRotation();
	m_pNavigation->Set_CurrentIdx(m_pOwnerTransform->Get_Position());
}

void CUrdState_Executed::Die()
{
	if (false == m_pUrd->Is_Phase2())
		return; 

	m_pUrd->Pause_Anim(999999.f);

	m_pUrd->Active_Dissolve();
	PLAY_SOUND(L"Urd_Dead", false, 1.f);
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
