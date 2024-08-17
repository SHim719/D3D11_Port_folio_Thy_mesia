#include "TwinBladeKnightState_Base.h"

#include "GameObject.h"

CTwinBladeKnightState_Base::CTwinBladeKnightState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CState_Base(pDevice, pContext)
{
}

HRESULT CTwinBladeKnightState_Base::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTwinBladeKnight = (CTwinBladeKnight*)pArg;

	m_pTargetTransform = m_pTwinBladeKnight->Get_Target()->Get_Transform();
	Safe_AddRef(m_pTargetTransform);

	Init_AttackDesc();

	return S_OK;
}

void CTwinBladeKnightState_Base::OnState_Start(void* pArg)
{
}

void CTwinBladeKnightState_Base::Update(_float fTimeDelta)
{

}

void CTwinBladeKnightState_Base::OnState_End()
{
}

void CTwinBladeKnightState_Base::OnHit(const ATTACKDESC& AttackDesc)
{
	if (0 == m_pTwinBladeKnight->Take_Damage(AttackDesc))
		m_pTwinBladeKnight->Change_State((_uint)TwinBladeKnight_State::State_Stunned_Start);
	else if (!m_pTwinBladeKnight->Is_Stanced() || IGNORE_STANCE == AttackDesc.ePlayerAttackType)
		m_pTwinBladeKnight->Change_State((_uint)TwinBladeKnight_State::State_Hit, const_cast<ATTACKDESC*>(&AttackDesc));

	//m_iHitCount = (m_iHitCount + 1) % (m_iMaxHitCount + 1);
	//
	//string strBloodEffect = m_iHitCount % 2 == 0 ? "Effect_Blood_R_Vill_F" : "Effect_Blood_L_Vill_F";
	//EFFECTMGR->Active_Effect(strBloodEffect, &m_pVillager_F->Get_EffectSpawnDesc());
	//
	//EFFECTMGR->Active_Effect("Effect_Enemy_Hit_Particle", &m_pVillager_F->Get_EffectSpawnDesc());
	//
	//static_cast<CMain_Camera*>(GET_CAMERA)->Play_CameraShake("Shaking_Hit");
}


void CTwinBladeKnightState_Base::Decide_State()
{
	_float fDist = XMVector3Length(m_pTargetTransform->Get_Position() - m_pOwnerTransform->Get_Position()).m128_f32[0];
	_int iRandNum = JoRandom::Random_Int(0, 2);
	
	if (0 == iRandNum && fDist < 3.f)
		Decide_Attack();
	else
		m_pTwinBladeKnight->Change_State((_uint)TwinBladeKnight_State::State_Walk);
}

void CTwinBladeKnightState_Base::Decide_Attack()
{
	_int 	iRandNum = JoRandom::Random_Int(0, 2);
	
	if (0 == iRandNum)
		m_pTwinBladeKnight->Change_State((_uint)TwinBladeKnight_State::State_ComboA);
	else if (1 == iRandNum)
		m_pTwinBladeKnight->Change_State((_uint)TwinBladeKnight_State::State_ComboB);
	else
		m_pTwinBladeKnight->Change_State((_uint)TwinBladeKnight_State::State_ComboC);
}

void CTwinBladeKnightState_Base::Free()
{
	__super::Free();

	Safe_Release(m_pTargetTransform);
}
