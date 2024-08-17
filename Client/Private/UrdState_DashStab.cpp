#include "UrdState_DashStab.h"

#include "Effect_Manager.h"

CUrdState_DashStab::CUrdState_DashStab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_DashStab::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Init_AttackDesc();

	return S_OK;
}

void CUrdState_DashStab::OnState_Start(void* pArg)
{
	m_pUrd->Set_LookTarget(true);
	m_pUrd->Set_Stanced(true);

	Reset_AttackIdx();
	m_pUrd->Update_AttackDesc();

	_int iRandNum = JoRandom::Random_Int(0, 1);
	if (iRandNum)
		m_pModel->Change_Animation(Urd_Attack03_DashSting_L);
	else
		m_pModel->Change_Animation(Urd_Attack04_DashSting_R);

	EFFECTMGR->Active_Effect("Effect_Urd_Step_Particle", &m_pUrd->Get_EffectSpawnDesc());
}

void CUrdState_DashStab::Update(_float fTimeDelta)
{
	if (false == m_pUrd->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CUrdState_DashStab::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		__super::Decide_State();
}

void CUrdState_DashStab::OnState_End()
{

}

void CUrdState_DashStab::Init_AttackDesc()
{
	ATTACKDESC AttackDesc{};
	AttackDesc.eEnemyAttackType = NORMAL;
	AttackDesc.iDamage = 87;

	m_AttackDescs.emplace_back(make_pair(CUrd::SWORD, AttackDesc));
}

void CUrdState_DashStab::Decide_State()
{
	_int iRandNum = JoRandom::Random_Int(0, 1);
	if (1 == iRandNum)
		ADD_EVENT(bind(&CUrdState_Base::Decide_Step, this));
}


CUrdState_DashStab* CUrdState_DashStab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_DashStab* pInstance = new CUrdState_DashStab(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_DashStab"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_DashStab::Free()
{
	__super::Free();
}
