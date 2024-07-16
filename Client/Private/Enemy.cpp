#include "Enemy.h"
#include "State_Base.h"
#include "Bone.h"
#include "Weapon.h"
#include "Player.h"

#include "UI_Manager.h"

#include "UI_EnemyBar.h"

CGameObject* CEnemy::s_pTarget = nullptr;

CEnemy::CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter(pDevice, pContext)
{
}

CEnemy::CEnemy(const CEnemy& rhs)
	: CCharacter(rhs)
	, m_eExecutionTag(rhs.m_eExecutionTag)
	, m_eSkillType(rhs.m_eSkillType)
	, m_iStunnedStateIdx(rhs.m_iStunnedStateIdx)
	, m_iDeathStateIdx(rhs.m_iDeathStateIdx)
	, m_iExecutionStateIdx(rhs.m_iExecutionStateIdx)
{

}

void CEnemy::Tick(_float fTimeDelta)
{
	if (m_bLookTarget)
	{
		m_pTransform->Rotation_Quaternion(
			JoMath::Slerp_TargetLook(m_pTransform->Get_GroundLook()
				, JoMath::Calc_GroundLook(s_pTarget->Get_Transform()->Get_Position(), m_pTransform->Get_Position())
				, m_fRotRate * fTimeDelta));
	}

	m_States[m_iState]->Update(fTimeDelta);

	if (m_bAdjustNaviY)
		Compute_YPos();

	__super::Update_Colliders();

	__super::Tick_Weapons(fTimeDelta);

	if (m_pEnemyBar && m_pStats->Is_Hit())
		m_pEnemyBar->Tick(fTimeDelta);

	m_pModel->Play_Animation(fTimeDelta);

}
void CEnemy::LateTick(_float fTimeDelta)
{
	m_States[m_iState]->Late_Update(fTimeDelta);

	__super::LateTick_Weapons(fTimeDelta);

	if (m_pEnemyBar && m_pStats->Is_Hit())
		m_pEnemyBar->LateTick(fTimeDelta);

	if (m_bNoRender)
		return;

	if (m_pGameInstance->In_WorldFrustum(m_pTransform->Get_Position() + XMLoadFloat4(&m_vCullingOffset), m_fCullingRadius))
	{
#ifdef _DEBUG
		m_pGameInstance->Add_RenderComponent(m_pCollider);
		m_pGameInstance->Add_RenderComponent(m_pHitBoxCollider);
#endif
		
		if (m_pEnemyBar && m_pStats->Is_Hit())
			m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, m_pEnemyBar);

		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	}
}

HRESULT CEnemy::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pModel->SetUp_BoneMatrices(m_pShader)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModel->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/

		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, 0)))
			return E_FAIL;
	}

	return S_OK;
}
void CEnemy::Percept_Target()
{
}

void CEnemy::SetState_Death()
{
	Change_State(m_iDeathStateIdx);

	Safe_Release(m_pEnemyBar);
}

void CEnemy::SetState_Executed(void* pArg)
{
	Change_State(m_iExecutionStateIdx, pArg);

	Safe_Release(m_pEnemyBar);
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
	if (true == m_bStunnedMarked)
		return;

	m_bStunnedMarked = true;

	ATTACHDESC AttachDesc = {};
	AttachDesc.pParentTransform = m_pTransform;
	AttachDesc.pAttachBone = Find_SpineBone();

	UIMGR->Active_UI("UI_StunnedMark", &AttachDesc);
}

void CEnemy::InActive_StunnedMark()
{
	if (false == m_bStunnedMarked)
		return;

	m_bStunnedMarked = false;

	UIMGR->Inactive_UI("UI_StunnedMark");
}

void CEnemy::Free()
{
	__super::Free();

	Safe_Release(m_pStats);
	Safe_Release(m_pEnemyBar);
}
