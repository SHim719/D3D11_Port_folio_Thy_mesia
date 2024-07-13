#include "PlayerState_Execution_TwinBladeKnight.h"

#include "Enemy.h"

CPlayerState_Execution_TwinBladeKnight::CPlayerState_Execution_TwinBladeKnight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Execution_TwinBladeKnight::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayerState_Execution_TwinBladeKnight::OnState_Start(void* pArg)
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

	m_pModel->Change_Animation(Corvus_VSHArmorLV1_Execution, 0.f);
}

void CPlayerState_Execution_TwinBladeKnight::Update(_float fTimeDelta)
{
}

void CPlayerState_Execution_TwinBladeKnight::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		m_pMain_Camera->Reset_State();
		m_pExecutionEnemy->SetState_Death();
		Setup_RootRotation();
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
	}

}


void CPlayerState_Execution_TwinBladeKnight::OnState_End()
{
	Safe_Release(m_pExecutionEnemy);
	m_pPlayer->Set_CanNextState(true);
	m_pPlayer->Set_Adjust_NaviY(true);
}


CPlayerState_Execution_TwinBladeKnight* CPlayerState_Execution_TwinBladeKnight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Execution_TwinBladeKnight* pInstance = new CPlayerState_Execution_TwinBladeKnight(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Execution_TwinBladeKnight"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Execution_TwinBladeKnight::Free()
{
	__super::Free();
}
