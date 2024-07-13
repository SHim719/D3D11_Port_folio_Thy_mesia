#include "HalberdKnightState_Hit.h"

CHalberdKnightState_Hit::CHalberdKnightState_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CHalberdKnightState_Base(pDevice, pContext)
{
}

HRESULT CHalberdKnightState_Hit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CHalberdKnightState_Hit::OnState_Start(void* pArg)
{
	m_pHalberdKnight->Set_LookTarget(true);

	ATTACKDESC* pAttackDesc = (ATTACKDESC*)pArg;
	
	switch (pAttackDesc->ePlayerAttackType)
	{
	case ATTACK_NORMAL:
	case PLUNDER:
		m_pModel->Change_Animation(HArmorLV1_Halberds_HurtS_FL + m_iHitCount % 2);
		break;
	case IGNORE_STANCE:
		m_pModel->Change_Animation(HArmorLV1_Halberds_HurtCounter);
		break;
	}

}

void CHalberdKnightState_Hit::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CHalberdKnightState_Hit::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_State();
}

void CHalberdKnightState_Hit::OnState_End()
{
	m_iHitCount = 0;
}

void CHalberdKnightState_Hit::OnHit(const ATTACKDESC& AttackDesc)
{
	if (0 == m_pHalberdKnight->Take_Damage(AttackDesc))
	{
		m_pHalberdKnight->Change_State((_uint)HalberdKnight_State::State_Stunned_Start);
		return;
	}

	m_iHitCount = (m_iHitCount + 1) % (m_iMaxHitCount + 1);

	_int iRandNum = JoRandom::Random_Int(0, m_iMaxHitCount);

	if (iRandNum <= m_iHitCount)
		Decide_Attack();
	else
		OnState_Start(const_cast<ATTACKDESC*>(&AttackDesc));
}

CHalberdKnightState_Hit* CHalberdKnightState_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CHalberdKnightState_Hit* pInstance = new CHalberdKnightState_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHalberdKnightState_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHalberdKnightState_Hit::Free()
{
	__super::Free();
}
