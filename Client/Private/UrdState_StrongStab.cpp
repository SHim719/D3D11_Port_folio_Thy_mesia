#include "UrdState_StrongStab.h"

#include "Effect_Manager.h"

CUrdState_StrongStab::CUrdState_StrongStab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_StrongStab::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Init_AttackDesc();

	return S_OK;
}

void CUrdState_StrongStab::OnState_Start(void* pArg)
{
	m_pUrd->Set_LookTarget(true);
	m_pUrd->Set_Stanced(true);
	m_pUrd->Resize_WeaponCollider(false);

	Reset_AttackIdx();
	m_pUrd->Update_AttackDesc();

	m_pModel->Change_Animation(Urd_Attack02);
}

void CUrdState_StrongStab::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CUrdState_StrongStab::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pUrd->Change_State((_uint)UrdState::State_Walk);
}


void CUrdState_StrongStab::OnState_End()
{
	m_pUrd->Resize_WeaponCollider(true);
}

void CUrdState_StrongStab::Init_AttackDesc()
{
	ATTACKDESC AttackDesc{};
	AttackDesc.eEnemyAttackType = KNOCKBACK;
	AttackDesc.iDamage = 123;
	AttackDesc.strShakingTag = "Shaking_Big_Hit";

	m_AttackDescs.emplace_back(make_pair(CUrd::SWORD, AttackDesc));
}

void CUrdState_StrongStab::Decide_State()
{
	_int iRandNum = JoRandom::Random_Int(0, 1);

	if (1 == iRandNum)
		ADD_EVENT(bind(&CUrdState_Base::Decide_Step, this));
}

CUrdState_StrongStab* CUrdState_StrongStab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_StrongStab* pInstance = new CUrdState_StrongStab(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_StrongStab"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_StrongStab::Free()
{
	__super::Free();
}
