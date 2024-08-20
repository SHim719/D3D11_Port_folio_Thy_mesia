#include "HalberdKnight.h"
#include "HalberdKnight_States.h"
#include "Weapon.h"
#include "Bone.h"
#include "PerceptionBounding.h"

#include "UI_EnemyBar.h"


CHalberdKnight::CHalberdKnight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CHalberdKnight::CHalberdKnight(const CHalberdKnight& rhs)
	: CEnemy(rhs)
{
}

HRESULT CHalberdKnight::Initialize_Prototype()
{
	m_iTag = (_uint)TAG_ENEMY;
	
	m_eSkillType = SKILLTYPE::SPEAR;

	m_iDeathStateIdx = (_uint)HalberdKnight_State::State_Death;
	m_iStunnedStateIdx = (_uint)HalberdKnight_State::State_Stunned_Loop;
	m_iStunnedStartStateIdx = (_uint)HalberdKnight_State::State_Stunned_Start;

	return S_OK;
}

HRESULT CHalberdKnight::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	if (FAILED(Ready_Weapon()))
		return E_FAIL;

	if (FAILED(Ready_Stats()))
		return E_FAIL;

	if (FAILED(Ready_UI()))
		return E_FAIL;

	Bind_KeyFrames();

	m_pGameInstance->Add_Clone(GET_CURLEVEL, L"PerceptionBounding", L"Prototype_PerceptionBounding", this);

	m_tEffectSpawnDesc.pParentTransform = m_pTransform;
	m_tEffectSpawnDesc.pParentModel = m_pModel;

	m_bLookTarget = false;

	m_fCullingRadius = 1.5f;
	XMStoreFloat4(&m_vCullingOffset, XMVectorSet(0.f, 1.2f, 0.f, 0.f));

	Change_State((_uint)HalberdKnight_State::State_Idle);
	m_pModel->Set_AnimPlay();

	return S_OK;
}

void CHalberdKnight::Bind_KeyFrames()
{
	m_pModel->Bind_Func("Active_WeaponCollider", bind(&CWeapon::Set_Active_Collider, m_Weapons[SPEAR], true));
	m_pModel->Bind_Func("Inactive_WeaponCollider", bind(&CWeapon::Set_Active_Collider, m_Weapons[SPEAR], false));
	m_pModel->Bind_Func("Enable_LookTarget", bind(&CEnemy::Set_LookTarget, this, true));
	m_pModel->Bind_Func("Disable_LookTarget", bind(&CEnemy::Set_LookTarget, this, false));
	m_pModel->Bind_Func("Enable_Stanced", bind(&CCharacter::Set_Stanced, this, true));
	m_pModel->Bind_Func("Disable_Stanced", bind(&CCharacter::Set_Stanced, this, false));
	m_pModel->Bind_Func("Update_AttackDesc", bind(&CCharacter::Update_AttackDesc, this));
	m_pModel->Bind_Func("ChangeToNextAttackAnim", bind(&CHalberdKnight::Change_To_NextComboAnim, this));
	m_pModel->Bind_Func("Active_Dissolve", bind(&CGameObject::Active_Dissolve, this));

	m_pModel->Bind_Func("Sound_AttackVoice", bind(&CGameInstance::Play_RandomSound, m_pGameInstance, L"Halberds_Voice", 1, 5, false, 0.6f));
	m_pModel->Bind_Func("Sound_Whoosh", bind(&CGameInstance::Play_RandomSound, m_pGameInstance, L"Spear_Whoosh", 1, 2, false, 0.5f));

}

void CHalberdKnight::Change_To_NextComboAnim()
{
	ADD_EVENT(bind(&CModel::Change_Animation, m_pModel, m_pModel->Get_CurrentAnimIndex() + 1, 0.1f, true));
}

void CHalberdKnight::Percept_Target()
{
	Change_State((_uint)HalberdKnight_State::State_Walk);
}

HRESULT CHalberdKnight::Ready_Components(void* pArg)
{
	LOADOBJDESC* pLoadDesc = (LOADOBJDESC*)pArg;

	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 2.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Model_HalberdKnight"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	CCollider::COLLIDERDESC Desc;
	Desc.eType = CCollider::SPHERE;
	Desc.pOwner = this;
	Desc.vCenter = { 0.f, 1.3f, 0.f };
	Desc.vSize = { 1.f, 0.f, 0.f };
	Desc.vRotation = { 0.f, 0.f, 0.f };
	Desc.strCollisionLayer = "Enemy";
	Desc.bActive = true;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("Collider"), (CComponent**)&m_pCollider, &Desc)))
		return E_FAIL;

	Desc.eType = CCollider::SPHERE;
	Desc.pOwner = this;
	Desc.vCenter = { 0.f, 1.3f, 0.f };
	Desc.vSize = { 1.5f, 0.f, 0.0f };
	Desc.vRotation = { 0.f, 0.f, 0.f };
	Desc.strCollisionLayer = "Enemy_HitBox";
	Desc.bActive = true;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("HitBox"), (CComponent**)&m_pHitBoxCollider, &Desc)))
		return E_FAIL;

	CNavigation::NAVIGATION_DESC NaviDesc{};
	NaviDesc.iCurrentCellIndex = pLoadDesc->iNaviIdx;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Navigation"), TEXT("Navigation"), (CComponent**)&m_pNavigation, &NaviDesc)))
		return E_FAIL;

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pLoadDesc->WorldMatrix));

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Dissolve"), TEXT("Dissolve_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHalberdKnight::Ready_States()
{
	m_States.resize((_uint)HalberdKnight_State::State_End);
	m_States[(_uint)HalberdKnight_State::State_Idle] = CHalberdKnightState_Idle::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)HalberdKnight_State::State_Walk] = CHalberdKnightState_Walk::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)HalberdKnight_State::State_Attack1] = CHalberdKnightState_Attack1::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)HalberdKnight_State::State_Attack2] = CHalberdKnightState_Attack2::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)HalberdKnight_State::State_Attack3] = CHalberdKnightState_Attack3::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)HalberdKnight_State::State_Stunned_Start] = CHalberdKnightState_Stunned_Start::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)HalberdKnight_State::State_Stunned_Loop] = CHalberdKnightState_Stunned_Loop::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)HalberdKnight_State::State_Death] = CHalberdKnightState_Death::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)HalberdKnight_State::State_Hit] = CHalberdKnightState_Hit::Create(m_pDevice, m_pContext, this);

	return S_OK;
}

HRESULT CHalberdKnight::Ready_Weapon()
{
	m_Weapons.resize(WEAPON_END);

	CCollider::COLLIDERDESC ColliderDesc = {};
	ColliderDesc.eType = CCollider::OBB;
	ColliderDesc.pOwner = this;
	ColliderDesc.vCenter = { 0.f, 0.f, 0.f };
	ColliderDesc.vSize = { 3.0f, 0.1f, 0.1f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };
	ColliderDesc.bActive = false;
	ColliderDesc.strCollisionLayer = "Enemy_Weapon";

	CWeapon::WEAPONDESC WeaponDesc;
	WeaponDesc.iTag = (_uint)TAG_ENEMY_WEAPON;
	WeaponDesc.iLevelID = GET_CURLEVEL;
	WeaponDesc.pParentTransform = m_pTransform;
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_r_pivot");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Spear";
	WeaponDesc.pOwner = this;
	WeaponDesc.pColliderDesc = &ColliderDesc;

	m_Weapons[SPEAR] = static_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[SPEAR])
		return E_FAIL;

	return S_OK;
}

HRESULT CHalberdKnight::Ready_Stats()
{
	ENEMYDESC EnemyDesc;
	EnemyDesc.wstrEnemyName = L"할버드기사";
	EnemyDesc.iMaxHp = 100;

	m_pStats = CEnemyStats::Create(EnemyDesc);

	return S_OK;
}

HRESULT CHalberdKnight::Ready_UI()
{
	CUI_EnemyBar::UIENEMYBARDESC EnemyBarDesc;
	EnemyBarDesc.pStats = m_pStats;
	EnemyBarDesc.vOffset.y = 2.5f;
	EnemyBarDesc.pOwnerTransform = m_pTransform;

	m_pEnemyBar = static_cast<CUI_EnemyBar*>(m_pGameInstance->Clone_GameObject(L"Prototype_EnemyBar", &EnemyBarDesc));

	return S_OK;
}


CHalberdKnight* CHalberdKnight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHalberdKnight* pInstance = new CHalberdKnight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHalberdKnight"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHalberdKnight::Clone(void* pArg)
{
	CHalberdKnight* pInstance = new CHalberdKnight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CHalberdKnight"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHalberdKnight::Free()
{
	__super::Free();
}
