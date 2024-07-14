#include "TwinBladeKnightState_ParryAttack.h"

#include "Player.h"

CTwinBladeKnightState_ParryAttack::CTwinBladeKnightState_ParryAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTwinBladeKnightState_Base(pDevice, pContext)
{
}

HRESULT CTwinBladeKnightState_ParryAttack::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTwinBladeKnightState_ParryAttack::OnState_Start(void* pArg)
{
	static_cast<CPlayer*>(m_pTwinBladeKnight->Get_Target())->SetState_Parried();

	Reset_AttackIdx();

	m_pTwinBladeKnight->Set_LookTarget(true);
	m_pTwinBladeKnight->Set_Stanced(true);
	m_pTwinBladeKnight->Update_AttackDesc();

	m_pModel->Change_Animation(LArmor_TwinSwords_RaidAttack1);
}

void CTwinBladeKnightState_ParryAttack::Update(_float fTimeDelta)
{
	if (false == m_pTwinBladeKnight->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CTwinBladeKnightState_ParryAttack::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_State();
}

void CTwinBladeKnightState_ParryAttack::OnState_End()
{
}

void CTwinBladeKnightState_ParryAttack::Init_AttackDesc()
{
	m_AttackDescs.reserve(9);

	ATTACKDESC AttackDesc;
	AttackDesc.eEnemyAttackType = NORMAL;
	AttackDesc.iDamage = 30;
	AttackDesc.pAttacker = m_pTwinBladeKnight;

	m_AttackDescs.emplace_back(CTwinBladeKnight::BLADE_L, AttackDesc);
	m_AttackDescs.emplace_back(CTwinBladeKnight::BLADE_R, AttackDesc);
	m_AttackDescs.emplace_back(CTwinBladeKnight::BLADE_L, AttackDesc);
	m_AttackDescs.emplace_back(CTwinBladeKnight::BLADE_L, AttackDesc);
	m_AttackDescs.emplace_back(CTwinBladeKnight::BLADE_R, AttackDesc);
	m_AttackDescs.emplace_back(CTwinBladeKnight::BLADE_L, AttackDesc);
	m_AttackDescs.emplace_back(CTwinBladeKnight::BLADE_R, AttackDesc);
	m_AttackDescs.emplace_back(CTwinBladeKnight::BLADE_L, AttackDesc);
	m_AttackDescs.emplace_back(CTwinBladeKnight::BLADE_R, AttackDesc);
	
	AttackDesc.iDamage = 40;
	AttackDesc.eEnemyAttackType = BIG_HIT;
	m_AttackDescs.emplace_back(CTwinBladeKnight::BLADE_R, AttackDesc);
}


CTwinBladeKnightState_ParryAttack* CTwinBladeKnightState_ParryAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTwinBladeKnightState_ParryAttack* pInstance = new CTwinBladeKnightState_ParryAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTwinBladeKnightState_ParryAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTwinBladeKnightState_ParryAttack::Free()
{
	__super::Free();
}
