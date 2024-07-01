#include "JokerState_ComboA.h"

CJokerState_ComboA::CJokerState_ComboA(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CJokerState_Base(pDevice, pContext)
{
}

HRESULT CJokerState_ComboA::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CJokerState_ComboA::OnState_Start(void* pArg)
{
	Reset_AttackIdx();
	m_pJoker->Set_LookTarget(true);
	m_pJoker->Update_AttackDesc();

	m_pModel->Change_Animation(Joker_ComboA01);
}

void CJokerState_ComboA::Update(_float fTimeDelta)
{
	if (!m_pJoker->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CJokerState_ComboA::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_State();
}

void CJokerState_ComboA::OnState_End()
{

}

void CJokerState_ComboA::Init_AttackDesc()
{
	m_AttackDescs.reserve(2);

	ATTACKDESC AttackDesc;
	AttackDesc.eEnemyAttackType = NORMAL;
	AttackDesc.iDamage = 70;
	AttackDesc.pAttacker = m_pJoker;

	m_AttackDescs.emplace_back(CJoker::HAMMER, AttackDesc);

	AttackDesc.eEnemyAttackType = SEMIKNOCKBACK;

	m_AttackDescs.emplace_back(CJoker::HAMMER, AttackDesc);
	
}

CJokerState_ComboA* CJokerState_ComboA::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CJokerState_ComboA* pInstance = new CJokerState_ComboA(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CJokerState_ComboA"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJokerState_ComboA::Free()
{
	__super::Free();
}
