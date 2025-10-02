#include "JokerState_TurnAttack_R.h"

CJokerState_TurnAttack_R::CJokerState_TurnAttack_R(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CJokerState_Base(pDevice, pContext)
{
}

HRESULT CJokerState_TurnAttack_R::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CJokerState_TurnAttack_R::OnState_Start(void* pArg)
{
	Reset_AttackIdx();
	m_pJoker->Set_LookTarget(false);
	m_pJoker->Update_AttackDesc();

	m_pModel->Change_Animation(Joker_TurnAttackR);
}

void CJokerState_TurnAttack_R::Update(_float fTimeDelta)
{
	if (!m_pJoker->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);

}

void CJokerState_TurnAttack_R::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_State();
	
	m_pOwnerTransform->Turn_Quaternion(m_pModel->Get_DeltaRootQuat(), XMLoadFloat4x4(&m_OffsetMatrix));
	//m_pOwnerTransform->Rotation_Quaternion(m_pModel->Get_DeltaRootQuat());
}

void CJokerState_TurnAttack_R::OnState_End()
{
}

void CJokerState_TurnAttack_R::Init_AttackDesc()
{
	m_AttackDescs.reserve(1);

	ATTACKDESC AttackDesc;
	AttackDesc.eEnemyAttackType = NORMAL;
	AttackDesc.iDamage = 70;
	AttackDesc.pAttacker = m_pJoker;

	m_AttackDescs.emplace_back(CJoker::HAMMER, AttackDesc);
}

CJokerState_TurnAttack_R* CJokerState_TurnAttack_R::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CJokerState_TurnAttack_R* pInstance = new CJokerState_TurnAttack_R(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CJokerState_TurnAttack_R"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJokerState_TurnAttack_R::Free()
{
	__super::Free();
}
