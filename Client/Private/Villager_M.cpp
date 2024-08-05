#include "Villager_M.h"
#include "VillagerM_States.h"
#include "Weapon.h"
#include "Bone.h"
#include "PerceptionBounding.h"

#include "UI_Manager.h"
#include "UI_EnemyBar.h"

CVillager_M::CVillager_M(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CVillager_M::CVillager_M(const CVillager_M& rhs)
	: CEnemy(rhs)
{
}

HRESULT CVillager_M::Initialize_Prototype()
{
	m_iTag = (_uint)TAG_ENEMY;
	m_eSkillType = SKILLTYPE::AXE;

	m_iDeathStateIdx = (_uint)VillagerM_State::State_Death;
	m_iStunnedStateIdx = (_uint)VillagerM_State::State_Stunned_Loop;

	return S_OK;
}

HRESULT CVillager_M::Initialize(void* pArg)
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

	m_bLookTarget = false;

	m_tEffectSpawnDesc.pParentTransform = m_pTransform;
	m_tEffectSpawnDesc.pParentModel = m_pModel;

	m_fCullingRadius = 1.5f;
	XMStoreFloat4(&m_vCullingOffset, XMVectorSet(0.f, 1.2f, 0.f, 0.f));

	Change_State((_uint)VillagerM_State::State_Idle);
	m_pModel->Set_AnimPlay();

	return S_OK;
}


void CVillager_M::Bind_KeyFrames()
{
	m_pModel->Bind_Func("Active_WeaponCollider", bind(&CWeapon::Set_Active_Collider, m_Weapons[AXE], true));
	m_pModel->Bind_Func("Inactive_WeaponCollider", bind(&CWeapon::Set_Active_Collider, m_Weapons[AXE], false));
	m_pModel->Bind_Func("Enable_LookTarget", bind(&CEnemy::Set_LookTarget, this, true));
	m_pModel->Bind_Func("Disable_LookTarget", bind(&CEnemy::Set_LookTarget, this, false));
	m_pModel->Bind_Func("Enable_Stanced", bind(&CCharacter::Set_Stanced, this, true));
	m_pModel->Bind_Func("Disable_Stanced", bind(&CCharacter::Set_Stanced, this, false));
	m_pModel->Bind_Func("Update_AttackDesc", bind(&CCharacter::Update_AttackDesc, this));
}

void CVillager_M::Percept_Target()
{
	_int iRandNum = JoRandom::Random_Int(0, 1);

	if (iRandNum)
		Change_State((_uint)VillagerM_State::State_Walk);
	else
		Change_State((_uint)VillagerM_State::State_Attack);
}


HRESULT CVillager_M::Ready_Components(void* pArg)
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

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Model_Villager_M"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	CCollider::COLLIDERDESC Desc;
	Desc.eType = CCollider::SPHERE;
	Desc.pOwner = this;
	Desc.vCenter = { 0.f, 1.3f, 0.f };
	Desc.vSize = { 1.2f, 0.f, 0.f };
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

	CNavigation::NAVIGATION_DESC NaviDesc;
	NaviDesc.iCurrentCellIndex = pLoadDesc->iNaviIdx;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Navigation"), TEXT("Navigation"), (CComponent**)&m_pNavigation, &NaviDesc)))
		return E_FAIL;

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pLoadDesc->WorldMatrix));

	return S_OK;
}

HRESULT CVillager_M::Ready_States()
{
	m_States.resize((_uint)VillagerM_State::State_End);

	m_States[(_uint)VillagerM_State::State_Idle] = CVillagerMState_Idle::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VillagerM_State::State_Walk] = CVillagerMState_Walk::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VillagerM_State::State_Attack] = CVillagerMState_Attack::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VillagerM_State::State_Hit] = CVillagerMState_Hit::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VillagerM_State::State_Death] = CVillagerMState_Death::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VillagerM_State::State_Stunned_Start] = CVillagerMState_Stunned_Start::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VillagerM_State::State_Stunned_Loop] = CVillagerMState_Stunned_Loop::Create(m_pDevice, m_pContext, this);

	return S_OK;
}

HRESULT CVillager_M::Ready_Weapon()
{
	m_Weapons.resize(WEAPON_END);

	CCollider::COLLIDERDESC ColliderDesc = {};
	ColliderDesc.eType = CCollider::SPHERE;
	ColliderDesc.pOwner = this;
	ColliderDesc.vCenter = { 0.8f, 0.f, 0.f };
	ColliderDesc.vSize = { 0.8f, 0.1f, 0.1f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };
	ColliderDesc.bActive = false;
	ColliderDesc.strCollisionLayer = "Enemy_Weapon";

	CWeapon::WEAPONDESC WeaponDesc;
	WeaponDesc.iTag = (_uint)TAG_ENEMY_WEAPON;
	WeaponDesc.iLevelID = GET_CURLEVEL;
	WeaponDesc.pParentTransform = m_pTransform;
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_r");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Villager_M_Axe";
	WeaponDesc.pOwner = this;
	WeaponDesc.pColliderDesc = &ColliderDesc;

	m_Weapons[AXE] = static_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[AXE])
		return E_FAIL;

	return S_OK;
}

HRESULT CVillager_M::Ready_Stats()
{
	ENEMYDESC EnemyDesc;
	EnemyDesc.wstrEnemyName = L"∏ÛΩ∫≈Õ2";
	EnemyDesc.iMaxHp = 150;

	m_pStats = CEnemyStats::Create(EnemyDesc);

	return S_OK;
}

HRESULT CVillager_M::Ready_UI()
{
	CUI_EnemyBar::UIENEMYBARDESC EnemyBarDesc;
	EnemyBarDesc.vOffset.y = 2.5f;
	EnemyBarDesc.pOwnerTransform = m_pTransform;
	EnemyBarDesc.pStats = m_pStats;

	m_pEnemyBar = static_cast<CUI_EnemyBar*>(m_pGameInstance->Clone_GameObject(L"Prototype_EnemyBar", &EnemyBarDesc));

	return S_OK;
}


CVillager_M* CVillager_M::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVillager_M* pInstance = new CVillager_M(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CVillager_M"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVillager_M::Clone(void* pArg)
{
	CVillager_M* pInstance = new CVillager_M(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CVillager_M"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillager_M::Free()
{
	__super::Free();
}
