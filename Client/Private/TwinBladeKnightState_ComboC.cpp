#include "TwinBladeKnightState_ComboC.h"

#include "Player.h"

CTwinBladeKnightState_ComboC::CTwinBladeKnightState_ComboC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTwinBladeKnightState_Base(pDevice, pContext)
{
}

HRESULT CTwinBladeKnightState_ComboC::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTwinBladeKnightState_ComboC::OnState_Start(void* pArg)
{
	Reset_AttackIdx();

	m_pTwinBladeKnight->Set_LookTarget(true);
	m_pTwinBladeKnight->Set_Stanced(true);
	m_pTwinBladeKnight->Update_AttackDesc();

	m_pModel->Change_Animation(LArmor_TwinSwords_ComboC01);
}

void CTwinBladeKnightState_ComboC::Update(_float fTimeDelta)
{
	if (false == m_pTwinBladeKnight->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CTwinBladeKnightState_ComboC::Late_Update(_float fTimeDelta)
{
	if (true == m_pModel->Is_AnimComplete())
	{
		_int iRandNum = JoRandom::Random_Int(0, 5);
		if (iRandNum < 5)
			Decide_State();
		else
			m_pTwinBladeKnight->Change_State((_uint)TwinBladeKnight_State::State_Dodge);

	}
}

void CTwinBladeKnightState_ComboC::OnState_End()
{
}

void CTwinBladeKnightState_ComboC::Init_AttackDesc()
{
	m_AttackDescs.reserve(5);

	ATTACKDESC AttackDesc;
	AttackDesc.eEnemyAttackType = NORMAL;
	AttackDesc.iDamage = 25;
	AttackDesc.pAttacker = m_pTwinBladeKnight;

	m_AttackDescs.emplace_back(CTwinBladeKnight::BLADE_L, AttackDesc);
	m_AttackDescs.emplace_back(CTwinBladeKnight::BLADE_R, AttackDesc);
	m_AttackDescs.emplace_back(CTwinBladeKnight::BLADE_R, AttackDesc);
	m_AttackDescs.emplace_back(CTwinBladeKnight::BLADE_L, AttackDesc);
	

	AttackDesc.eEnemyAttackType = SEMIKNOCKBACK;
	AttackDesc.iDamage = 50;
	m_AttackDescs.emplace_back(CTwinBladeKnight::BLADE_R, AttackDesc);

}


CTwinBladeKnightState_ComboC* CTwinBladeKnightState_ComboC::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTwinBladeKnightState_ComboC* pInstance = new CTwinBladeKnightState_ComboC(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTwinBladeKnightState_ComboC"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTwinBladeKnightState_ComboC::Free()
{
	__super::Free();
}
