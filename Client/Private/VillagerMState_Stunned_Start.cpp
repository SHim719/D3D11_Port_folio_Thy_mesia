#include "VillagerMState_Stunned_Start.h"

#include "Main_Camera.h"

#include "Player.h"


CVillagerMState_Stunned_Start::CVillagerMState_Stunned_Start(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVillagerMState_Base(pDevice, pContext)
{
}

HRESULT CVillagerMState_Stunned_Start::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVillagerMState_Stunned_Start::OnState_Start(void* pArg)
{
	m_pVillager_M->Set_LookTarget(false);
	m_pOwnerTransform->LookAt2D(m_pTargetTransform->Get_Position());

	RIMLIGHTDESC RimDesc{};
	RimDesc.bColorLerp = true;
	RimDesc.fDuration = 1.5f;
	RimDesc.fRimPower = 1.f;
	RimDesc.fRimStrength = 3.f;
	RimDesc.vRimColor = { 0.f, 1.f, 0.6f, 1.f };

	m_pVillager_M->Active_RimLight(RimDesc);

	static_cast<CPlayer*>(m_pVillager_M->Get_Target())->Add_StunnedEnemy(m_pVillager_M);

	static_cast<CMain_Camera*>(GET_CAMERA)->Play_CameraShake("Shaking_Execution");

	m_pModel->Change_Animation(LV1Villager_M_HurtStunStart);
}

void CVillagerMState_Stunned_Start::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CVillagerMState_Stunned_Start::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pVillager_M->Change_State((_uint)VillagerM_State::State_Stunned_Loop);
		
}

void CVillagerMState_Stunned_Start::OnState_End()
{
}

void CVillagerMState_Stunned_Start::OnHit(const ATTACKDESC& AttackDesc)
{
	
}

CVillagerMState_Stunned_Start* CVillagerMState_Stunned_Start::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CVillagerMState_Stunned_Start* pInstance = new CVillagerMState_Stunned_Start(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CVillagerMState_Stunned_Start"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillagerMState_Stunned_Start::Free()
{
	__super::Free();
}
