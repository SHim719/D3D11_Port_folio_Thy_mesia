#include "JokerState_WheelWind_Loop.h"

CJokerState_WheelWind_Loop::CJokerState_WheelWind_Loop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CJokerState_Base(pDevice, pContext)
{
}

HRESULT CJokerState_WheelWind_Loop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CJokerState_WheelWind_Loop::OnState_Start(void* pArg)
{
	Reset_AttackIdx();
	m_pJoker->Set_LookTarget(true);
	m_pJoker->Update_AttackDesc();

	m_pModel->Change_Animation(Joker_WheelAttackLoop);
}

void CJokerState_WheelWind_Loop::Update(_float fTimeDelta)
{
	if (!m_pJoker->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CJokerState_WheelWind_Loop::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		if (m_iNowWheelWindIdx == m_iMaxAttackIdx)
		{
			m_pJoker->Change_State((_uint)JokerState::State_WheelWind_End);
		}
		else
		{
			OnState_Start(nullptr);
			++m_iNowWheelWindIdx;
		}
	}
}

void CJokerState_WheelWind_Loop::OnState_End()
{
	m_iNowWheelWindIdx = 0;
}

void CJokerState_WheelWind_Loop::Init_AttackDesc()
{
	m_AttackDescs.reserve(1);

	ATTACKDESC AttackDesc;
	AttackDesc.eEnemyAttackType = NORMAL;
	AttackDesc.iDamage = 40;
	AttackDesc.pAttacker = m_pJoker;

	m_AttackDescs.emplace_back(CJoker::HAMMER, AttackDesc);
}

CJokerState_WheelWind_Loop* CJokerState_WheelWind_Loop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CJokerState_WheelWind_Loop* pInstance = new CJokerState_WheelWind_Loop(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CJokerState_WheelWind_Loop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJokerState_WheelWind_Loop::Free()
{
	__super::Free();
}
