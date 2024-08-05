#include "VillagerFState_Hit.h"

#include "Main_Camera.h"

CVillagerFState_Hit::CVillagerFState_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVillagerFState_Base(pDevice, pContext)
{
}

HRESULT CVillagerFState_Hit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVillagerFState_Hit::OnState_Start(void* pArg)
{
	m_pVillager_F->Set_LookTarget(true);

	m_pModel->Change_Animation(LV1Villager_F_HurtM_FL + m_iHitCount % 2);
}

void CVillagerFState_Hit::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CVillagerFState_Hit::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_State();
}

void CVillagerFState_Hit::OnState_End()
{
	m_iHitCount = 0;
}

void CVillagerFState_Hit::OnHit(const ATTACKDESC& AttackDesc)
{
	if (0 == m_pVillager_F->Take_Damage(AttackDesc))
	{
		m_pVillager_F->Change_State((_uint)VillagerF_State::State_Stunned_Start);
		return;
	}

	m_iHitCount = (m_iHitCount + 1) % (m_iMaxHitCount + 1);

	string strBloodEffect = m_iHitCount % 2 == 0 ? "Effect_Blood_R_Vill_F" : "Effect_Blood_L_Vill_F";
	EFFECTMGR->Active_Effect(strBloodEffect, &m_pVillager_F->Get_EffectSpawnDesc());

	EFFECTMGR->Active_Effect("Effect_Enemy_Hit_Particle", &m_pVillager_F->Get_EffectSpawnDesc());

	static_cast<CMain_Camera*>(GET_CAMERA)->Play_CameraShake("Shaking_Hit");

	_int iRandNum = JoRandom::Random_Int(0, m_iMaxHitCount);

	if (iRandNum <= m_iHitCount)
		Decide_Attack();
	else
		OnState_Start(nullptr);
}

CVillagerFState_Hit* CVillagerFState_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CVillagerFState_Hit* pInstance = new CVillagerFState_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CVillagerFState_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillagerFState_Hit::Free()
{
	__super::Free();
}
