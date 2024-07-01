#include "JokerState_WheelWind_End.h"

CJokerState_WheelWind_End::CJokerState_WheelWind_End(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CJokerState_Base(pDevice, pContext)
{
}

HRESULT CJokerState_WheelWind_End::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CJokerState_WheelWind_End::OnState_Start(void* pArg)
{
	Reset_AttackIdx();
	m_pJoker->Set_LookTarget(true);
	m_pJoker->Update_AttackDesc();

	m_pModel->Change_Animation(Joker_WheelAttackEnd_1);
}

void CJokerState_WheelWind_End::Update(_float fTimeDelta)
{
	if (!m_pJoker->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CJokerState_WheelWind_End::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_State();
}

void CJokerState_WheelWind_End::OnState_End()
{

}

void CJokerState_WheelWind_End::Init_AttackDesc()
{
	m_AttackDescs.reserve(2);

	ATTACKDESC AttackDesc;
	AttackDesc.eEnemyAttackType = NORMAL;
	AttackDesc.iDamage = 40;
	AttackDesc.pAttacker = m_pJoker;

	m_AttackDescs.emplace_back(CJoker::HAMMER, AttackDesc);

	AttackDesc.iDamage = 80;
	AttackDesc.eEnemyAttackType = KNOCKDOWN;

	m_AttackDescs.emplace_back(CJoker::HAMMER, AttackDesc);
}

CJokerState_WheelWind_End* CJokerState_WheelWind_End::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CJokerState_WheelWind_End* pInstance = new CJokerState_WheelWind_End(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CJokerState_WheelWind_End"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJokerState_WheelWind_End::Free()
{
	__super::Free();
}
