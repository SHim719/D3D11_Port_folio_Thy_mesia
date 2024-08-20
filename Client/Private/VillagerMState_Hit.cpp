#include "VillagerMState_Hit.h"

#include "Main_Camera.h"

CVillagerMState_Hit::CVillagerMState_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVillagerMState_Base(pDevice, pContext)
{
}

HRESULT CVillagerMState_Hit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVillagerMState_Hit::OnState_Start(void* pArg)
{
	m_pOwnerTransform->LookAt2D(m_pTargetTransform->Get_Position());

	m_pModel->Change_Animation(LV1Villager_M_HurtS_FL + m_iHitCount % 2);
}

void CVillagerMState_Hit::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CVillagerMState_Hit::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_State();
}

void CVillagerMState_Hit::OnState_End()
{
	m_iHitCount = 0;
}

void CVillagerMState_Hit::OnHit(const ATTACKDESC& AttackDesc)
{
	if (0 == m_pVillager_M->Take_Damage(AttackDesc))
	{
		m_pVillager_M->Change_State((_uint)VillagerM_State::State_Stunned_Start);
		return;
	}

	m_iHitCount = (m_iHitCount + 1) % (m_iMaxHitCount + 1);

	string strBloodEffect = m_iHitCount % 2 == 0 ? "Effect_Blood_R_Vill_M" : "Effect_Blood_L_Vill_M";
	EFFECTMGR->Active_Effect(strBloodEffect, &m_pVillager_M->Get_EffectSpawnDesc());
	EFFECTMGR->Active_Effect("Effect_Enemy_Hit_Particle", &m_pVillager_M->Get_EffectSpawnDesc());

	static_cast<CMain_Camera*>(GET_CAMERA)->Play_CameraShake("Shaking_Hit");

	_int iRandNum = JoRandom::Random_Int(0, m_iMaxHitCount);

	if (iRandNum <= m_iHitCount)
		m_pVillager_M->Change_State((_uint)VillagerM_State::State_Attack);
	else
		OnState_Start(nullptr);

	Play_HitSound();
}

CVillagerMState_Hit* CVillagerMState_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CVillagerMState_Hit* pInstance = new CVillagerMState_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CVillagerMState_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillagerMState_Hit::Free()
{
	__super::Free();
}
