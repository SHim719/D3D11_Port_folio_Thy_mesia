#include "PlayerState_Execution_Elite.h"

#include "Enemy.h"

#include "UI_Manager.h"

CPlayerState_Execution_Elite::CPlayerState_Execution_Elite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Execution_Elite::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayerState_Execution_Elite::OnState_Start(void* pArg)
{
	m_pPlayer->Set_CanNextState(false);

	CBone* pAnimTargetBone = m_pModel->Get_Bone("AnimTargetPoint");
	if (nullptr == pAnimTargetBone)
		return;

	CBone* pCameraBone = m_pModel->Get_Bone("camera");
	if (nullptr == pCameraBone)
		return;

	ATTACHDESC AttachDesc{};
	AttachDesc.pAttachBone = pAnimTargetBone;
	AttachDesc.pParentTransform = m_pOwnerTransform;

	m_pExecutionEnemy = (CEnemy*)pArg;
	m_pExecutionEnemy->SetState_Executed(&AttachDesc);
	Safe_AddRef(m_pExecutionEnemy);

	AttachDesc.pAttachBone = pCameraBone;
	m_pMain_Camera->SetState_Cutscene(AttachDesc);

	m_pPlayer->Set_Adjust_NaviY(false);

	
}

void CPlayerState_Execution_Elite::Update(_float fTimeDelta)
{
}

void CPlayerState_Execution_Elite::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		if (false == m_pExecutionEnemy->Is_Death())
			m_pExecutionEnemy->SetState_Death();

		m_pMain_Camera->Reset_CutsceneState();

		Setup_RootRotation();

		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
	}
		
}


void CPlayerState_Execution_Elite::OnState_End()
{
	Safe_Release(m_pExecutionEnemy);
	m_pPlayer->Set_CanNextState(true);
	m_pPlayer->Set_Adjust_NaviY(true);
}


CPlayerState_Execution_Elite* CPlayerState_Execution_Elite::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Execution_Elite* pInstance = new CPlayerState_Execution_Elite(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Execution_Elite"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Execution_Elite::Free()
{
	__super::Free();
	Safe_Release(m_pExecutionEnemy);
}
