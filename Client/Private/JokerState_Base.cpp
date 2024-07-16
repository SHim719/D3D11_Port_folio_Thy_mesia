#include "JokerState_Base.h"

#include "GameObject.h"

CJokerState_Base::CJokerState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CState_Base(pDevice, pContext)
{
}

HRESULT CJokerState_Base::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pJoker = (CJoker*)pArg;

	m_pTargetTransform = m_pJoker->Get_Target()->Get_Transform();
	Safe_AddRef(m_pTargetTransform);

	Init_AttackDesc();

	return S_OK;
}

void CJokerState_Base::OnState_Start(void* pArg)
{
}

void CJokerState_Base::Update(_float fTimeDelta)
{

}

void CJokerState_Base::OnState_End()
{
}

void CJokerState_Base::OnHit(const ATTACKDESC& AttackDesc)
{
	if (0 == m_pJoker->Take_Damage(AttackDesc))
		m_pJoker->Change_State((_uint)JokerState::State_Stunned_Start);
}

void CJokerState_Base::Decide_State()
{
	_int iRandNum = JoRandom::Random_Int(0, 1);
	
	// 0일때는 유휴상태, 1일때는 공격상태
	
	if (iRandNum)
		Decide_Attack();
	else
		m_pJoker->Change_State((_uint)JokerState::State_Walk);
}

void CJokerState_Base::Decide_Attack()
{
	_float fDist = XMVector3Length(m_pTargetTransform->Get_Position() - m_pOwnerTransform->Get_Position()).m128_f32[0];

	if (fDist < 2.5f)
	{
		_float fToTargetAngle = JoMath::Calc_AngleToTarget(m_pOwnerTransform->Get_Position(), m_pTargetTransform->Get_Position(),
			m_pOwnerTransform->Get_Look());
		_float fDegree = XMConvertToDegrees(fToTargetAngle);

		if (XM_PIDIV2 < fToTargetAngle && fToTargetAngle <= XM_PI)
		{
			m_pJoker->Change_State((_uint)JokerState::State_TurnAttack_R);
			return;
		}
		else if (-XM_PI <= fToTargetAngle && fToTargetAngle < -XM_PIDIV2)
		{
			m_pJoker->Change_State((_uint)JokerState::State_TurnAttack_L);
			return;
		}
	}

	if (fDist > 5.f)
	{
		m_pJoker->Change_State((_uint)JokerState::State_JumpAttack);
	}
	else
	{
		_int iRandNum = JoRandom::Random_Int(0, 3);
	
		switch (iRandNum)
		{
		case 0:
			m_pJoker->Change_State((_uint)JokerState::State_ComboA);
			break;
		case 1:
			m_pJoker->Change_State((_uint)JokerState::State_ComboB);
			break;
		case 2:
			m_pJoker->Change_State((_uint)JokerState::State_WheelWind_Start);
			break;
		case 3:
			m_pJoker->Change_State((_uint)JokerState::State_StrongAttack);
			break;
		}
		
	}
}


void CJokerState_Base::Free()
{
	__super::Free();

	Safe_Release(m_pTargetTransform);
}
