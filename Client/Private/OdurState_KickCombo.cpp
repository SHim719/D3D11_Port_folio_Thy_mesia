#include "OdurState_KickCombo.h"

COdurState_KickCombo::COdurState_KickCombo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_KickCombo::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModel->Bind_Func("Odur_ChangeAnimKickToDisappear", bind(&COdurState_KickCombo::Change_To_Disappear, this));

	return S_OK;
}

void COdurState_KickCombo::OnState_Start(void* pArg)
{
	Reset_AttackIdx();
	m_pOdur->Set_Stanced(true);
	m_pOdur->Set_LookTarget(true);

	for (_uint i = 0; i < 2; ++i)
		m_pOdur->Update_AttackDesc();

	PLAY_SOUND(L"Odur_KickCombo", false, 0.6f);
	m_pModel->Change_Animation(Magician_KickCombo);
}

void COdurState_KickCombo::Update(_float fTimeDelta)
{
	if (false == m_pOdur->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void COdurState_KickCombo::OnState_End()
{

}

void COdurState_KickCombo::Init_AttackDesc()
{
	m_AttackDescs.reserve(2);

	ATTACKDESC AttackDesc;
	AttackDesc.pAttacker = m_pOdur;
	AttackDesc.eEnemyAttackType = NORMAL;
	AttackDesc.iDamage = 42;

	m_AttackDescs.emplace_back(COdur::FOOT_R, AttackDesc);

	AttackDesc.eEnemyAttackType = KNOCKBACK;
	AttackDesc.iDamage = 83;
	AttackDesc.strShakingTag = "Shaking_Big_Hit";

	m_AttackDescs.emplace_back(COdur::SWORD, AttackDesc);
}

void COdurState_KickCombo::Change_To_Disappear()
{
	m_pOdur->Swap_Bone();
	ADD_EVENT(bind(&COdur::Change_State, m_pOdur, (_uint)OdurState::State_DisappearMove, &m_iDisappearDir));
}

COdurState_KickCombo* COdurState_KickCombo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_KickCombo* pInstance = new COdurState_KickCombo(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_KickCombo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_KickCombo::Free()
{
	__super::Free();
}
