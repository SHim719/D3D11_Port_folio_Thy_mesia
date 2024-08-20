#include "VillagerFState_Stunned_Start.h"
#include "Main_Camera.h"

#include "Player.h"

CVillagerFState_Stunned_Start::CVillagerFState_Stunned_Start(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVillagerFState_Base(pDevice, pContext)
{
}

HRESULT CVillagerFState_Stunned_Start::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVillagerFState_Stunned_Start::OnState_Start(void* pArg)
{
	m_pOwnerTransform->LookAt2D(m_pTargetTransform->Get_Position());
	m_pVillager_F->Set_LookTarget(false);

	static_cast<CPlayer*>(m_pVillager_F->Get_Target())->Add_StunnedEnemy(m_pVillager_F);

	RIMLIGHTDESC RimDesc{};
	RimDesc.bColorLerp = true;
	RimDesc.fDuration = 0.5f;
	RimDesc.fRimPower = 1.f;
	RimDesc.fRimStrength = 3.f;
	RimDesc.vRimColor = { 0.f, 1.f, 0.6f, 1.f };

	PLAY_SOUND(L"Normal_KillChance", false, 1.f);

	static_cast<CMain_Camera*>(GET_CAMERA)->Play_CameraShake("Shaking_Execution");

	m_pVillager_F->Active_RimLight(RimDesc);

	m_pModel->Change_Animation(LV1Villager_F_HurtStunStart);

}

void CVillagerFState_Stunned_Start::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CVillagerFState_Stunned_Start::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pVillager_F->Change_State((_uint)VillagerF_State::State_Stunned_Loop);

}

void CVillagerFState_Stunned_Start::OnState_End()
{
}

void CVillagerFState_Stunned_Start::OnHit(const ATTACKDESC& AttackDesc)
{

}

CVillagerFState_Stunned_Start* CVillagerFState_Stunned_Start::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CVillagerFState_Stunned_Start* pInstance = new CVillagerFState_Stunned_Start(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CVillagerFState_Stunned_Start"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillagerFState_Stunned_Start::Free()
{
	__super::Free();
}
