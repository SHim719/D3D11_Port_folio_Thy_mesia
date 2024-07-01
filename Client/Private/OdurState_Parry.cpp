#include "OdurState_Parry.h"

#include "Player.h"

COdurState_Parry::COdurState_Parry(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_Parry::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void COdurState_Parry::OnState_Start(void* pArg)
{
	Reset_AttackIdx();
	m_pOdur->Set_LookTarget(true);
	m_pOdur->Set_Stanced(true);
	
	static_cast<CPlayer*>(m_pOdur->Get_Target())->SetState_Parried();

	Decide_Animation();

	m_pOdur->Update_AttackDesc();
}

void COdurState_Parry::Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		Decide_State();
		return;
	}

	if (false == m_pOdur->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void COdurState_Parry::OnState_End()
{
	
}

void COdurState_Parry::Init_AttackDesc()
{
	m_ParryAttackDescs[0].reserve(2);
	m_ParryAttackDescs[1].reserve(2);

	ATTACKDESC AtkDesc;
	AtkDesc.pAttacker = m_pOdur;
	AtkDesc.eEnemyAttackType = NORMAL;
	m_ParryAttackDescs[0].emplace_back(COdur::CANE, AtkDesc);
	m_ParryAttackDescs[1].emplace_back(COdur::SWORD, AtkDesc);
	m_ParryAttackDescs[1].emplace_back(COdur::SWORD, AtkDesc);
	
	AtkDesc.eEnemyAttackType = KNOCKBACK;
	m_ParryAttackDescs[0].emplace_back(COdur::FOOT_L, AtkDesc);
}

void COdurState_Parry::Decide_Animation()
{
	_int iRandNum = JoRandom::Random_Int(0, 1);
	
	if (iRandNum)
		m_pModel->Change_Animation(Magician_Parry01); // µÚ·Î°¬´Ù°¡ ½»»è
	else
		m_pModel->Change_Animation(Magician_ParryAttack01); // KICK

	m_AttackDescs = m_ParryAttackDescs[iRandNum];
}

COdurState_Parry* COdurState_Parry::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_Parry* pInstance = new COdurState_Parry(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_Parry"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_Parry::Free()
{
	__super::Free();
}
