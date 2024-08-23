#include "UrdState_ExtraAttack.h"

#include "Effect_Manager.h"

CUrdState_ExtraAttack::CUrdState_ExtraAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_ExtraAttack::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Init_AttackDesc();

	return S_OK;
}

void CUrdState_ExtraAttack::OnState_Start(void* pArg)
{
	m_pUrd->Set_LookTarget(false);
	m_pUrd->Set_Stanced(true);

	Reset_AttackIdx();
	m_pUrd->Update_AttackDesc();

	if ((_uint)UrdState::State_DefaultAttack == m_pUrd->Get_PrevState())
		m_pModel->Change_Animation(Urd_Attack06);
	else
		m_pModel->Change_Animation(Urd_Attack07);

}

		

void CUrdState_ExtraAttack::Update(_float fTimeDelta)
{
	if (false == m_pUrd->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CUrdState_ExtraAttack::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		__super::Decide_State();
}


void CUrdState_ExtraAttack::OnState_End()
{
	if (m_pGameInstance->Is_Playing(L"Urd_Attack6"))
		m_pGameInstance->Stop(L"Urd_Attack6");
	if (m_pGameInstance->Is_Playing(L"Urd_Attack7"))
		m_pGameInstance->Stop(L"Urd_Attack7");
		
}

void CUrdState_ExtraAttack::Init_AttackDesc()
{
	ATTACKDESC AttackDesc{};
	AttackDesc.pAttacker = m_pUrd;
	AttackDesc.eEnemyAttackType = KNOCKBACK;
	AttackDesc.iDamage = 113;
	AttackDesc.strShakingTag = "Shaking_Big_Hit";

	m_AttackDescs.emplace_back(make_pair(CUrd::SWORD, AttackDesc));

}

CUrdState_ExtraAttack* CUrdState_ExtraAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_ExtraAttack* pInstance = new CUrdState_ExtraAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_ExtraAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_ExtraAttack::Free()
{
	__super::Free();
}
