#include "Joker.h"
#include "Joker_States.h"
#include "Weapon.h"
#include "Bone.h"
#include "PerceptionBounding.h"

#include "UI_Manager.h"
#include "UI_EnemyBar.h"

#include "Main_Camera.h"

#include "Player.h"
#include "PlayerStats.h"

CJoker::CJoker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CJoker::CJoker(const CJoker& rhs)
	: CEnemy(rhs)
{
}

HRESULT CJoker::Initialize_Prototype()
{
	m_iTag = (_uint)TAG_ENEMY;
	m_eExecutionTag = JOKER;
	m_eSkillType = SKILLTYPE::HAMMER;

	m_iDeathStateIdx = (_uint)JokerState::State_Death;
	m_iExecutionStateIdx = (_uint)JokerState::State_Executed_Start;
	m_iStunnedStateIdx = (_uint)JokerState::State_Stunned_Loop;

	m_iSoulCount = 1000;

	return S_OK;
}

HRESULT CJoker::Initialize(void* pArg)
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
	Bind_KeyFrameSounds();

	m_pGameInstance->Add_Clone(GET_CURLEVEL, L"PerceptionBounding", L"Prototype_PerceptionBounding", this);

	m_bLookTarget = false;
	m_bStanced = true;

	m_tEffectSpawnDesc.pParentTransform = m_pTransform;
	m_tEffectSpawnDesc.pParentModel = m_pModel;

	m_fRotRate = 2.f;

	XMStoreFloat4(&m_vCullingOffset, XMVectorSet(0.f, 1.5f, 0.f, 0.f));

	Change_State((_uint)JokerState::State_Idle);
	m_pModel->Set_AnimPlay();

	return S_OK;
}

void CJoker::Tick(_float fTimeDelta)
{
	if (KEY_DOWN(eKeyCode::N))
	{
		Change_State((_uint)JokerState::State_TurnAttack_R);
	}

	__super::Tick(fTimeDelta);		
}

HRESULT CJoker::Render()
{
	CEnemy::Render();

	return S_OK;
}

void CJoker::OnDeath()
{
	CPlayerStats* pStats = static_cast<CPlayer*>(s_pTarget)->Get_PlayerStats();

	if (SKILLTYPE::SKILLTYPE_END == m_eSkillType || pStats->Is_SkillActived(m_eSkillType))
		return;

	pStats->Active_Skill(m_eSkillType);
	pStats->Obtain_Key();

	vector<SKILLTYPE> PopupResources;
	PopupResources.emplace_back(m_eSkillType);
	PopupResources.emplace_back(SKILLTYPE::NONE);

	UIMGR->Active_UI("UI_Popup", &PopupResources);
}

void CJoker::Bind_KeyFrames()
{
	m_pModel->Bind_Func("Active_WeaponCollider", bind(&CWeapon::Set_Active_Collider, m_Weapons[HAMMER], true));
	m_pModel->Bind_Func("Inactive_WeaponCollider", bind(&CWeapon::Set_Active_Collider, m_Weapons[HAMMER], false));
	m_pModel->Bind_Func("Enable_LookTarget", bind(&CEnemy::Set_LookTarget, this, true));
	m_pModel->Bind_Func("Disable_LookTarget", bind(&CEnemy::Set_LookTarget, this, false));
	m_pModel->Bind_Func("Enable_Stanced", bind(&CCharacter::Set_Stanced, this, true));
	m_pModel->Bind_Func("Disable_Stanced", bind(&CCharacter::Set_Stanced, this, false));
	m_pModel->Bind_Func("Update_AttackDesc", bind(&CCharacter::Update_AttackDesc, this));
	m_pModel->Bind_Func("ChangeToNextAttack", bind(&CJoker::Change_To_NextComboAnim, this));
	m_pModel->Bind_Func("StrongAttack_Impact", bind(&CJoker::StrongAttack_Impact, this));
}

void CJoker::Bind_KeyFrameSounds()
{
	m_pModel->Bind_Func("Sound_HammerWhoosh", bind(&CGameInstance::Play_RandomSound, m_pGameInstance, L"Hammer_Whoosh", 1, 2, false, 0.6f));
	m_pModel->Bind_Func("Sound_Vocal_Swing", bind(&CGameInstance::Play_RandomSound, m_pGameInstance, L"Joker_Vocal_Swing", 1, 2, false, 0.7f));
	m_pModel->Bind_Func("Sound_Vocal_StrongAttack", bind(&CGameInstance::Play, m_pGameInstance, L"Joker_Vocal_StrongAttack", false, 0.7f));
	m_pModel->Bind_Func("Sound_Vocal_ShockImpact", bind(&CGameInstance::Play, m_pGameInstance, L"Sound_Vocal_ShockImpact", false, 0.7f));
	m_pModel->Bind_Func("Sound_Step", bind(&CGameInstance::Play_RandomSound, m_pGameInstance, L"FootStep_Heavy", 1, 2, false, 0.7f));

}


void CJoker::Percept_Target()
{
	Change_State((_uint)JokerState::State_Walk);
}

void CJoker::Change_To_NextComboAnim()
{
	ADD_EVENT(bind(&CModel::Change_Animation, m_pModel, m_pModel->Get_CurrentAnimIndex() + 1, 0.1f, true));
}

void CJoker::StrongAttack_Impact()
{
	static_cast<CMain_Camera*>(GET_CAMERA)->Play_CameraShake("Shaking_PW_Hammer");
	EFFECTMGR->Active_Effect("Effect_Joker_StrongAttack_Impact", &m_tEffectSpawnDesc);
	PLAY_SOUND(L"Joker_Shock_Impact", false, 1.f);
}

HRESULT CJoker::Ready_Components(void* pArg)
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

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Model_Joker"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	CCollider::COLLIDERDESC Desc;
	Desc.eType = CCollider::SPHERE;
	Desc.pOwner = this;
	Desc.vCenter = { 0.f, 1.5f, 0.f };
	Desc.vSize = { 2.f, 0.f, 0.f };
	Desc.vRotation = { 0.f, 0.f, 0.f };
	Desc.strCollisionLayer = "Enemy";
	Desc.bActive = true;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("Collider"), (CComponent**)&m_pCollider, &Desc)))
		return E_FAIL;

	Desc.eType = CCollider::SPHERE;
	Desc.pOwner = this;
	Desc.vCenter = { 0.f, 1.5f, 0.f };
	Desc.vSize = { 2.3f, 0.f, 0.0f };
	Desc.vRotation = { 0.f, 0.f, 0.f };
	Desc.strCollisionLayer = "Enemy_HitBox";
	Desc.bActive = true;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("HitBox"), (CComponent**)&m_pHitBoxCollider, &Desc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Navigation"), TEXT("Navigation"), (CComponent**)&m_pNavigation, &(pLoadDesc->iNaviIdx))))
		return E_FAIL;

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pLoadDesc->WorldMatrix));

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Dissolve"), TEXT("Dissolve_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CJoker::Ready_States()
{
	m_States.resize((_uint)JokerState::State_End);
	
	m_States[(_uint)JokerState::State_Idle] = CJokerState_Idle::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)JokerState::State_Walk] = CJokerState_Walk::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)JokerState::State_Hit] = CJokerState_Hit::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)JokerState::State_ComboA] = CJokerState_ComboA::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)JokerState::State_ComboB] = CJokerState_ComboB::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)JokerState::State_TurnAttack_R] = CJokerState_TurnAttack_R::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)JokerState::State_TurnAttack_L] = CJokerState_TurnAttack_L::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)JokerState::State_WheelWind_Start] = CJokerState_WheelWind_Start::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)JokerState::State_WheelWind_Loop] = CJokerState_WheelWind_Loop::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)JokerState::State_WheelWind_End] = CJokerState_WheelWind_End::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)JokerState::State_JumpAttack] = CJokerState_JumpAttack::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)JokerState::State_StrongAttack] = CJokerState_StrongAttack::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)JokerState::State_ShockAttack] = CJokerState_ShockAttack::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)JokerState::State_Stunned_Start] = CJokerState_Stunned_Start::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)JokerState::State_Stunned_Loop] = CJokerState_Stunned_Loop::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)JokerState::State_Executed_Start] = CJokerState_Executed_Start::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)JokerState::State_Death] = CJokerState_Executed_End::Create(m_pDevice, m_pContext, this);

	return S_OK;
}

HRESULT CJoker::Ready_Weapon()
{
	m_Weapons.resize(WEAPON_END);

	CCollider::COLLIDERDESC ColliderDesc = {};
	ColliderDesc.eType = CCollider::SPHERE;
	ColliderDesc.pOwner = this;
	ColliderDesc.vCenter = { 1.f, 0.f, 0.f };
	ColliderDesc.vSize = { 2.f, 0.1f, 0.1f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };
	ColliderDesc.bActive = false;
	ColliderDesc.strCollisionLayer = "Enemy_Weapon";

	CWeapon::WEAPONDESC WeaponDesc;
	WeaponDesc.iTag = (_uint)TAG_ENEMY_WEAPON;
	WeaponDesc.iLevelID = GET_CURLEVEL;
	WeaponDesc.pParentTransform = m_pTransform;
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_r_Hammer");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Joker_Hammer";
	WeaponDesc.pOwner = this;
	WeaponDesc.pColliderDesc = &ColliderDesc;

	m_Weapons[HAMMER] = static_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[HAMMER])
		return E_FAIL;

	return S_OK;
}

HRESULT CJoker::Ready_Stats()
{
	ENEMYDESC EnemyDesc;
	EnemyDesc.wstrEnemyName = L"Á¶Ä¿";
	EnemyDesc.iMaxHp = 300;

	m_pStats = CEnemyStats::Create(EnemyDesc);

	return S_OK;
}

HRESULT CJoker::Ready_UI()
{
	CUI_EnemyBar::UIENEMYBARDESC EnemyBarDesc;
	EnemyBarDesc.pStats = m_pStats;
	EnemyBarDesc.vOffset.y = 2.5f;
	EnemyBarDesc.pOwnerTransform = m_pTransform;

	m_pEnemyBar = static_cast<CUI_EnemyBar*>(m_pGameInstance->Clone_GameObject(L"Prototype_EnemyBar", &EnemyBarDesc));

	return S_OK;
}


CJoker* CJoker::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CJoker* pInstance = new CJoker(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CJoker"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CJoker::Clone(void* pArg)
{
	CJoker* pInstance = new CJoker(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CJoker"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJoker::Free()
{
	__super::Free();
}
