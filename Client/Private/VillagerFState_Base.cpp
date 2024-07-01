#include "VillagerFState_Base.h"

#include "GameObject.h"

CVillagerFState_Base::CVillagerFState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CState_Base(pDevice, pContext)
{
}

HRESULT CVillagerFState_Base::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pVillager_F = (CVillager_F*)pArg;

	m_pTargetTransform = m_pVillager_F->Get_Target()->Get_Transform();
	Safe_AddRef(m_pTargetTransform);

	Init_AttackDesc();

	return S_OK;
}

void CVillagerFState_Base::OnState_Start(void* pArg)
{
}

void CVillagerFState_Base::Update(_float fTimeDelta)
{

}

void CVillagerFState_Base::OnState_End()
{
}

void CVillagerFState_Base::OnHit(const ATTACKDESC& AttackDesc)
{
	if (0 == m_pVillager_F->Take_Damage(AttackDesc))
		m_pVillager_F->Change_State((_uint)VillagerF_State::State_Stunned_Start);
	else if (!m_pVillager_F->Is_Stanced())
		m_pVillager_F->Change_State((_uint)VillagerF_State::State_Hit);
}

void CVillagerFState_Base::Decide_State()
{
	_int iRandNum = JoRandom::Random_Int(0, 1);
	
	if (iRandNum)
		Decide_Attack();
	else
		m_pVillager_F->Change_State((_uint)VillagerF_State::State_Walk);
}

void CVillagerFState_Base::Decide_Attack()
{
	_int 	iRandNum = JoRandom::Random_Int(0, 2);
	
	if (0 == iRandNum)
		m_pVillager_F->Change_State((_uint)VillagerF_State::State_Attack1);
	else if (1 == iRandNum)
		m_pVillager_F->Change_State((_uint)VillagerF_State::State_Attack2);
	else
		m_pVillager_F->Change_State((_uint)VillagerF_State::State_Attack3);
}

void CVillagerFState_Base::Free()
{
	__super::Free();

	Safe_Release(m_pTargetTransform);
}
