#include "JokerState_JumpAttack.h"

CJokerState_JumpAttack::CJokerState_JumpAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CJokerState_Base(pDevice, pContext)
{
}

HRESULT CJokerState_JumpAttack::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CJokerState_JumpAttack::OnState_Start(void* pArg)
{
	Reset_AttackIdx();
	m_pJoker->Set_LookTarget(true);
	m_pJoker->Update_AttackDesc();
	m_pJoker->Set_Adjust_NaviY(false);

	m_pModel->Change_Animation(Joker_JumpAttack);

	XMStoreFloat4(&m_vPrevPos, m_pOwnerTransform->Get_Position());
}

void CJokerState_JumpAttack::Update(_float fTimeDelta)
{
	if (!m_pJoker->Is_CollPlayer())
	{
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
	}
}

void CJokerState_JumpAttack::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_State();
}

void CJokerState_JumpAttack::OnState_End()
{
	m_pJoker->Set_Adjust_NaviY(true);
}

void CJokerState_JumpAttack::Init_AttackDesc()
{
	m_AttackDescs.reserve(1);

	ATTACKDESC AttackDesc;
	AttackDesc.eEnemyAttackType = KNOCKDOWN;
	AttackDesc.iDamage = 84;
	AttackDesc.pAttacker = m_pJoker;



	m_AttackDescs.emplace_back(CJoker::HAMMER, AttackDesc);
}

CJokerState_JumpAttack* CJokerState_JumpAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CJokerState_JumpAttack* pInstance = new CJokerState_JumpAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CJokerState_JumpAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJokerState_JumpAttack::Free()
{
	__super::Free();
}
