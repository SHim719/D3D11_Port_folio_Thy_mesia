#include "OdurState_Base.h"

#include "GameObject.h"

COdurState_Base::COdurState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CState_Base(pDevice, pContext)
{
}

HRESULT COdurState_Base::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pOdur = (COdur*)pArg;

	m_pTargetTransform = m_pOdur->Get_Target()->Get_Transform();
	Safe_AddRef(m_pTargetTransform);

	return S_OK;
}

void COdurState_Base::OnState_Start(void* pArg)
{
}

void COdurState_Base::OnGoing(_float fTimeDelta)
{

}

void COdurState_Base::OnState_End()
{
}

void COdurState_Base::Decide_State()
{
	_int iRandNum = JoRandom::Random_Int(0, 9);

	if (iRandNum < 5)
		Decide_Attack();
	else if (iRandNum < 8)
		m_pOdur->Change_State((_uint)OdurState::State_DisappearMove);
	else
		m_pOdur->Change_State((_uint)OdurState::State_Walk);
}

void COdurState_Base::Decide_Attack()
{
	_int 	iRandNum = JoRandom::Random_Int(0, 2);

	if (0 == iRandNum)
		m_pOdur->Change_State((_uint)OdurState::State_CaneAttack1);
	else if (1 == iRandNum)
		m_pOdur->Change_State((_uint)OdurState::State_CaneAttack2);
	else
		m_pOdur->Change_State((_uint)OdurState::State_KickCombo);
}

void COdurState_Base::Free()
{
	__super::Free();

	Safe_Release(m_pTargetTransform);
}
