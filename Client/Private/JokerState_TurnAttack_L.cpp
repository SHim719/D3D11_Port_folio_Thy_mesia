#include "JokerState_TurnAttack_L.h"

CJokerState_TurnAttack_L::CJokerState_TurnAttack_L(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CJokerState_Base(pDevice, pContext)
{
}

HRESULT CJokerState_TurnAttack_L::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CJokerState_TurnAttack_L::OnState_Start(void* pArg)
{
	Reset_AttackIdx();
	m_pJoker->Set_LookTarget(false);
	m_pJoker->Update_AttackDesc();

	m_pModel->Change_Animation(Joker_TurnAttackL);
}

void CJokerState_TurnAttack_L::Update(_float fTimeDelta)
{
	if (!m_pJoker->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CJokerState_TurnAttack_L::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_State();
	
}

void CJokerState_TurnAttack_L::OnState_End()
{
	Setup_RootRotation();
}

void CJokerState_TurnAttack_L::Init_AttackDesc()
{
	m_AttackDescs.reserve(1);

	ATTACKDESC AttackDesc;
	AttackDesc.eEnemyAttackType = NORMAL;
	AttackDesc.iDamage = 70;
	AttackDesc.pAttacker = m_pJoker;

	m_AttackDescs.emplace_back(CJoker::HAMMER, AttackDesc);
}

CJokerState_TurnAttack_L* CJokerState_TurnAttack_L::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CJokerState_TurnAttack_L* pInstance = new CJokerState_TurnAttack_L(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CJokerState_TurnAttack_L"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJokerState_TurnAttack_L::Free()
{
	__super::Free();
}
