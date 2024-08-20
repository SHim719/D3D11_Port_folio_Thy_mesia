#include "TwinBladeKnightState_Hit.h"

#include "Main_Camera.h"

CTwinBladeKnightState_Hit::CTwinBladeKnightState_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTwinBladeKnightState_Base(pDevice, pContext)
{
}

HRESULT CTwinBladeKnightState_Hit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTwinBladeKnightState_Hit::OnState_Start(void* pArg)
{
	m_pTwinBladeKnight->Set_LookTarget(true);

	ATTACKDESC* pAttackDesc = (ATTACKDESC*)pArg;

	switch (pAttackDesc->ePlayerAttackType)
	{
	case ATTACK_NORMAL:
	case PLUNDER:
		m_pModel->Change_Animation(LArmor_TwinSwords_HurtM_FL + m_iHitCount % 2);
		break;
	case IGNORE_STANCE:
		m_pModel->Change_Animation(LArmor_TwinSwords_HurtCounter);
		break;
	}
}

void CTwinBladeKnightState_Hit::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CTwinBladeKnightState_Hit::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		_int iRandNum = JoRandom::Random_Int(0, 4);
		if (iRandNum)
			Decide_State();
		else
			m_pTwinBladeKnight->Change_State((_uint)TwinBladeKnight_State::State_Dodge);
	}
		
}

void CTwinBladeKnightState_Hit::OnState_End()
{
	m_iHitCount = 0;
}

void CTwinBladeKnightState_Hit::OnHit(const ATTACKDESC& AttackDesc)
{
	if (0 == m_pTwinBladeKnight->Take_Damage(AttackDesc))
	{
		m_pTwinBladeKnight->Change_State((_uint)TwinBladeKnight_State::State_Stunned_Start);
		return;
	}

	m_iHitCount = (m_iHitCount + 1) % (m_iMaxHitCount + 1);

	_int iRandNum = JoRandom::Random_Int(0, m_iMaxHitCount);

	if (iRandNum <= m_iHitCount)
	{
		Decide_Attack();
	}	
	else if (iRandNum > m_iMaxHitCount - 5 && ATTACK_NORMAL == AttackDesc.ePlayerAttackType)
	{
		m_pTwinBladeKnight->Change_State((_uint)TwinBladeKnight_State::State_ParryAttack);
		return;
	}
	else
	{
		OnState_Start(const_cast<ATTACKDESC*>(&AttackDesc));
	}

	EFFECTMGR->Active_Effect("Effect_TwinBlade_Hit", &m_pTwinBladeKnight->Get_EffectSpawnDesc());
	EFFECTMGR->Active_Effect("Effect_Enemy_Hit_Particle", &m_pTwinBladeKnight->Get_EffectSpawnDesc());
	
	static_cast<CMain_Camera*>(GET_CAMERA)->Play_CameraShake("Shaking_Hit");
		
	Play_HitSound();
}

CTwinBladeKnightState_Hit* CTwinBladeKnightState_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTwinBladeKnightState_Hit* pInstance = new CTwinBladeKnightState_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTwinBladeKnightState_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTwinBladeKnightState_Hit::Free()
{
	__super::Free();
}
