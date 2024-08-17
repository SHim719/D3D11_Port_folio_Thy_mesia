#include "TwinBladeKnightState_ComboB.h"

#include "Player.h"

CTwinBladeKnightState_ComboB::CTwinBladeKnightState_ComboB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTwinBladeKnightState_Base(pDevice, pContext)
{
}

HRESULT CTwinBladeKnightState_ComboB::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTwinBladeKnightState_ComboB::OnState_Start(void* pArg)
{
	Reset_AttackIdx();

	m_pTwinBladeKnight->Set_LookTarget(true);
	m_pTwinBladeKnight->Set_Stanced(true);
	m_pTwinBladeKnight->Update_AttackDesc();

	m_pModel->Change_Animation(LArmor_TwinSwords_ComboB01_1);
}

void CTwinBladeKnightState_ComboB::Update(_float fTimeDelta)
{
	if (false == m_pTwinBladeKnight->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CTwinBladeKnightState_ComboB::Late_Update(_float fTimeDelta)
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

void CTwinBladeKnightState_ComboB::OnState_End()
{
}

void CTwinBladeKnightState_ComboB::Init_AttackDesc()
{
	m_AttackDescs.reserve(3);

	ATTACKDESC AttackDesc;
	AttackDesc.eEnemyAttackType = NORMAL;
	AttackDesc.iDamage = 30;
	AttackDesc.pAttacker = m_pTwinBladeKnight;

	m_AttackDescs.emplace_back(CTwinBladeKnight::BLADE_R, AttackDesc);
	m_AttackDescs.emplace_back(CTwinBladeKnight::BLADE_L, AttackDesc);

	AttackDesc.iDamage = 35;
	AttackDesc.eEnemyAttackType = SEMIKNOCKBACK;

	m_AttackDescs.emplace_back(CTwinBladeKnight::BLADE_R, AttackDesc);
}


CTwinBladeKnightState_ComboB* CTwinBladeKnightState_ComboB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTwinBladeKnightState_ComboB* pInstance = new CTwinBladeKnightState_ComboB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTwinBladeKnightState_ComboB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTwinBladeKnightState_ComboB::Free()
{
	__super::Free();
}
