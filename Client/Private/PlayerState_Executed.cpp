#include "PlayerState_Executed.h"

#include "Main_Camera.h"

#include "Bone.h"

CPlayerState_Executed::CPlayerState_Executed(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Executed::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayerState_Executed::OnState_Start(void* pArg)
{
	m_pPlayer->Disable_NextState();
	m_pPlayer->Disable_Rotation();
	m_pPlayer->Inactive_AllWeaponColliders();
	m_pPlayer->Inactive_Colliders();

	m_ExecutedDesc = *(EXECUTEDDESC*)pArg;

	Safe_AddRef(m_ExecutedDesc.AttachDesc.pAttachBone);
	Safe_AddRef(m_ExecutedDesc.AttachDesc.pParentTransform);

	ATTACHDESC AttachDesc;
	AttachDesc.pAttachBone = m_pModel->Get_Bone("camera");
	AttachDesc.pParentTransform = m_pOwnerTransform;

	m_pMain_Camera->SetState_Cutscene(AttachDesc);

	m_pModel->Change_Animation(Corvus_VSMagician_SP1Hurt, 0.f);
}

void CPlayerState_Executed::OnGoing(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		m_pMain_Camera->Reset_State();

		//누워있게m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
		return;
	}

	_matrix CutsceneOffsetMatrix = XMMatrixRotationX(To_Radian(-90.f));

	m_pOwnerTransform->Attach_To_Bone(m_ExecutedDesc.AttachDesc.pAttachBone, m_ExecutedDesc.AttachDesc.pParentTransform, CutsceneOffsetMatrix);
}


void CPlayerState_Executed::OnState_End()
{
	m_pPlayer->Active_Colliders();
}

CPlayerState_Executed* CPlayerState_Executed::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Executed* pInstance = new CPlayerState_Executed(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Executed"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Executed::Free()
{
	__super::Free();

	Safe_Release(m_ExecutedDesc.AttachDesc.pAttachBone);
	Safe_Release(m_ExecutedDesc.AttachDesc.pParentTransform);
}
