#include "VillagerMState_Base.h"

#include "GameObject.h"

CVillagerMState_Base::CVillagerMState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CState_Base(pDevice, pContext)
{
}

HRESULT CVillagerMState_Base::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pVillager_M = (CVillager_M*)pArg;

	m_pTargetTransform = m_pVillager_M->Get_Target()->Get_Transform();
	Safe_AddRef(m_pTargetTransform);

	Init_AttackDesc();

	return S_OK;
}

void CVillagerMState_Base::OnState_Start(void* pArg)
{
}

void CVillagerMState_Base::Update(_float fTimeDelta)
{

}

void CVillagerMState_Base::OnState_End()
{
}

void CVillagerMState_Base::OnHit(const ATTACKDESC& AttackDesc)
{
	if (0 == m_pVillager_M->Take_Damage(AttackDesc))
		m_pVillager_M->Change_State((_uint)VillagerM_State::State_Stunned_Start);
	else if (!m_pVillager_M->Is_Stanced())
		m_pVillager_M->Change_State((_uint)VillagerM_State::State_Hit);
}

void CVillagerMState_Base::Decide_State()
{
	_int iRandNum = JoRandom::Random_Int(0, 1);

	if (iRandNum)
		m_pVillager_M->Change_State((_uint)VillagerM_State::State_Attack);
	else
		m_pVillager_M->Change_State((_uint)VillagerM_State::State_Walk);
}

void CVillagerMState_Base::Free()
{
	__super::Free();

	Safe_Release(m_pTargetTransform);
}
