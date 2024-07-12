#include "HalberdKnightState_Base.h"

#include "GameObject.h"

CHalberdKnightState_Base::CHalberdKnightState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CState_Base(pDevice, pContext)
{
}

HRESULT CHalberdKnightState_Base::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pHalberdKnight = (CHalberdKnight*)pArg;

	m_pTargetTransform = m_pHalberdKnight->Get_Target()->Get_Transform();
	Safe_AddRef(m_pTargetTransform);

	Init_AttackDesc();

	return S_OK;
}

void CHalberdKnightState_Base::OnState_Start(void* pArg)
{
}

void CHalberdKnightState_Base::Update(_float fTimeDelta)
{

}

void CHalberdKnightState_Base::OnState_End()
{
}

void CHalberdKnightState_Base::OnHit(const ATTACKDESC& AttackDesc)
{
	if (0 == m_pHalberdKnight->Take_Damage(AttackDesc))
		m_pHalberdKnight->Change_State((_uint)HalberdKnight_State::State_Stunned_Start);
	else if (!m_pHalberdKnight->Is_Stanced())
		m_pHalberdKnight->Change_State((_uint)HalberdKnight_State::State_Hit);
}

void CHalberdKnightState_Base::Decide_State()
{
	_int iRandNum = JoRandom::Random_Int(0, 1);

	if (iRandNum)
		Decide_Attack();
	else
		m_pHalberdKnight->Change_State((_uint)HalberdKnight_State::State_Walk);
}

void CHalberdKnightState_Base::Decide_Attack()
{
	_int 	iRandNum = JoRandom::Random_Int(0, 2);

	if (0 == iRandNum)
		m_pHalberdKnight->Change_State((_uint)HalberdKnight_State::State_Attack1);
	else if (1 == iRandNum)
		m_pHalberdKnight->Change_State((_uint)HalberdKnight_State::State_Attack2);
	else
		m_pHalberdKnight->Change_State((_uint)HalberdKnight_State::State_Attack3);
}

void CHalberdKnightState_Base::Free()
{
	__super::Free();

	Safe_Release(m_pTargetTransform);
}
