#include "JokerState_ComboB.h"

CJokerState_ComboB::CJokerState_ComboB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CJokerState_Base(pDevice, pContext)
{
}

HRESULT CJokerState_ComboB::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CJokerState_ComboB::OnState_Start(void* pArg)
{
	Reset_AttackIdx();
	m_pJoker->Set_LookTarget(true);
	m_pJoker->Update_AttackDesc();

	m_pModel->Change_Animation(Joker_ComboB01);
}

void CJokerState_ComboB::Update(_float fTimeDelta)
{
	if (!m_pJoker->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CJokerState_ComboB::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_State();
}

void CJokerState_ComboB::OnState_End()
{

}

void CJokerState_ComboB::Init_AttackDesc()
{
	m_AttackDescs.reserve(2);

	ATTACKDESC AttackDesc;
	AttackDesc.eEnemyAttackType = SEMIKNOCKBACK;
	AttackDesc.iDamage = 70;
	AttackDesc.pAttacker = m_pJoker;

	m_AttackDescs.emplace_back(CJoker::HAMMER, AttackDesc);

	AttackDesc.eEnemyAttackType = KNOCKDOWN;
	AttackDesc.iDamage = 90;

	m_AttackDescs.emplace_back(CJoker::HAMMER, AttackDesc);

}

CJokerState_ComboB* CJokerState_ComboB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CJokerState_ComboB* pInstance = new CJokerState_ComboB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CJokerState_ComboB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJokerState_ComboB::Free()
{
	__super::Free();
}
