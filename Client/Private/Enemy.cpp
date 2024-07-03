#include "Enemy.h"
#include "State_Base.h"
#include "Bone.h"
#include "UI_Manager.h"
#include "Weapon.h"
#include "Player.h"


CGameObject* CEnemy::s_pTarget = nullptr;

CEnemy::CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter(pDevice, pContext)
{
}

CEnemy::CEnemy(const CEnemy& rhs)
	: CCharacter(rhs)
	, m_eExecutionTag(rhs.m_eExecutionTag)
	, m_eSkillType(rhs.m_eSkillType)
{

}
void CEnemy::Percept_Target()
{
}

void CEnemy::SetState_Death()
{
}

void CEnemy::SetState_Executed(void* pArg)
{
}

_bool CEnemy::Is_Stunned()
{
	return false;
}

_bool CEnemy::Is_Death()
{
	return false;
}

CBone* CEnemy::Find_SpineBone()
{
	if (nullptr == m_pModel)
		return nullptr;

	CBone* pTargetBone = m_pModel->Get_Bone("Spine01");
	if (nullptr == pTargetBone)
		pTargetBone = m_pModel->Get_Bone("Bip001-Spine1");
	if (nullptr == pTargetBone)
		pTargetBone = m_pModel->Get_Bone("spine_01");
	if (nullptr == pTargetBone)
		pTargetBone = m_pModel->Get_Bone("Bone");

	return pTargetBone;
}


void CEnemy::OnCollisionEnter(CGameObject* pOther)
{
	if (TAG_PLAYER == pOther->Get_Tag())
		m_bCollPlayer = true;

	else if (TAG_PLAYER_WEAPON == pOther->Get_Tag())
	{
		const ATTACKDESC& AttackDesc = static_cast<CWeapon*>(pOther)->Get_AttackDesc();
		if (PLUNDER == AttackDesc.ePlayerAttackType)
			static_cast<CPlayer*>(s_pTarget)->SetState_Plunder(this);

		m_States[m_iState]->OnHit(AttackDesc);
	}
}

void CEnemy::OnCollisionExit(CGameObject* pOther)
{
	if (TAG_PLAYER == pOther->Get_Tag())
		m_bCollPlayer = false;
}

_int CEnemy::Take_Damage(const ATTACKDESC& AttackDesc)
{
	m_pStats->Increase_Hp(-AttackDesc.iDamage);
	return m_pStats->Increase_Mp(-AttackDesc.iPlagueDamage);
}

void CEnemy::Active_StunnedMark()
{
	if (ULLONG_MAX != m_iStunnedMarkIdx)
		return;

	ATTACHDESC AttachDesc = {};
	AttachDesc.pParentTransform = m_pTransform;
	AttachDesc.pAttachBone = Find_SpineBone();

	m_iStunnedMarkIdx = UIMGR->Active_UI("UI_StunnedMark", &AttachDesc);
}

void CEnemy::InActive_StunnedMark()
{
	if (ULLONG_MAX != m_iStunnedMarkIdx)
	{
		UIMGR->InActive_UI("UI_StunnedMark", m_iStunnedMarkIdx);
		m_iStunnedMarkIdx = ULLONG_MAX;
	}
}

void CEnemy::Free()
{
	__super::Free();

	Safe_Release(m_pStats);
}
