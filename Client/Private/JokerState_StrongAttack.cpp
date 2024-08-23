#include "JokerState_StrongAttack.h"

CJokerState_StrongAttack::CJokerState_StrongAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CJokerState_Base(pDevice, pContext)
{
}

HRESULT CJokerState_StrongAttack::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CJokerState_StrongAttack::OnState_Start(void* pArg)
{
	Reset_AttackIdx();
	m_pJoker->Set_LookTarget(true);
	m_pJoker->Update_AttackDesc();

	PLAY_SOUND(L"Joker_Vocal_StrongAttack", false, 1.f);
	
	m_pModel->Change_Animation(Joker_StrongAttack);
}

void CJokerState_StrongAttack::Update(_float fTimeDelta)
{
	if (!m_pJoker->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CJokerState_StrongAttack::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_State();
}

void CJokerState_StrongAttack::OnState_End()
{
}

void CJokerState_StrongAttack::Init_AttackDesc()
{
	m_AttackDescs.reserve(1);

	ATTACKDESC AttackDesc;
	AttackDesc.eEnemyAttackType = KNOCKDOWN;
	AttackDesc.iDamage = 84;
	AttackDesc.pAttacker = m_pJoker;

	m_AttackDescs.emplace_back(CJoker::HAMMER, AttackDesc);
}

CJokerState_StrongAttack* CJokerState_StrongAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CJokerState_StrongAttack* pInstance = new CJokerState_StrongAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CJokerState_StrongAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJokerState_StrongAttack::Free()
{
	__super::Free();
}
