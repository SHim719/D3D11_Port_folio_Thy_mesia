#include "UrdState_Base.h"

#include "GameObject.h"

#include "Main_Camera.h"

CUrdState_Base::CUrdState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_Base::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pUrd = (CUrd*)pArg;

	m_pTargetTransform = m_pUrd->Get_Target()->Get_Transform();
	Safe_AddRef(m_pTargetTransform);

	Init_AttackDesc();

	return S_OK;
}

void CUrdState_Base::OnState_Start(void* pArg)
{
}

void CUrdState_Base::Update(_float fTimeDelta)
{

}

void CUrdState_Base::OnState_End()
{
}

void CUrdState_Base::OnHit(const ATTACKDESC& AttackDesc)
{
	if (true == m_pUrd->Is_Invincible())
		return;


	if (0 == m_pUrd->Take_Damage(AttackDesc))
	{
		m_pUrd->Change_State((_uint)UrdState::State_Stunned_Start);
	}
	else if (!m_pUrd->Is_Stanced())
	{
		m_pUrd->Change_State((_uint)UrdState::State_Hit);
		return;
	}
	
	_int iRandNum = JoRandom::Random_Int(0, 1);
	string strBloodEffect = iRandNum == 0 ? "Effect_Blood_R_Odur" : "Effect_Blood_L_Odur";
	EFFECTMGR->Active_Effect(strBloodEffect, &m_pUrd->Get_EffectSpawnDesc());
	EFFECTMGR->Active_Effect("Effect_Enemy_Hit_Particle", &m_pUrd->Get_EffectSpawnDesc());
	
	static_cast<CMain_Camera*>(GET_CAMERA)->Play_CameraShake("Shaking_Hit");
}

void CUrdState_Base::Decide_State()
{
	int iRandNum = JoRandom::Random_Int(0, 4);

	if (iRandNum < 1 && true == m_pUrd->Can_EnableSkill())
	{
		_uint iState = 0;
		iRandNum = JoRandom::Random_Int(0, 2);

		if (iRandNum)
			iState = (_uint)UrdState::State_Skill3;
		else
			iState = (_uint)UrdState::State_Skill1;
		m_pUrd->Change_State(iState);
	}
	else if (iRandNum < 4)
	{
		Decide_Step();
	}
	else
		m_pUrd->Change_State((_uint)UrdState::State_Walk);

}

void CUrdState_Base::Decide_Step()
{
	_vector vUrdPos = m_pOwnerTransform->Get_Position();
	_vector vTargetPos = m_pTargetTransform->Get_Position();
	_float fDist = XMVector3Length(vTargetPos - vUrdPos).m128_f32[0];
	_int iRandNum = 0;

	if (fDist >= 4.f)
	{
		iRandNum = JoRandom::Random_Int(0, 1);
		switch (iRandNum)
		{
		case 0:
			m_pUrd->Change_State((_uint)UrdState::State_Step_F, &m_eDir);
			break;
		case 1:
			m_pUrd->Change_State((_uint)UrdState::State_Step_FLR, &m_eDir);
			break;
		}
	}
	else
	{
		m_pUrd->Change_State((_uint)UrdState::State_Step_LR, &m_eDir);
	}

}


void CUrdState_Base::Decide_Attack()
{
	_uint iState = 0;
	_int iRandNum = JoRandom::Random_Int(0, 3);

	switch (iRandNum)
	{
	case 0:
		iState = (_uint)UrdState::State_DefaultAttack;
		break;
	case 1:
		iState = (_uint)UrdState::State_TripleStab;
		break;
	case 2:
		iState = (_uint)UrdState::State_DashStab;
		break;
	case 3:
		iState = (_uint)UrdState::State_StrongStab;
		break;
	}
	m_pUrd->Change_State(iState);
}


void CUrdState_Base::Free()
{
	__super::Free();

	Safe_Release(m_pTargetTransform);
}
