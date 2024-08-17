#include "Odur.h"
#include "Odur_States.h"
#include "Weapon.h"
#include "Bone.h"

#include "Cutscene_Manager.h"
#include "UI_Manager.h"
#include "Effect_Manager.h"

#include "FadeScreen.h"

#include "LightObject.h"

#include "Player.h"

COdur::COdur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

COdur::COdur(const COdur& rhs)
	: CEnemy(rhs)
{
}

HRESULT COdur::Initialize_Prototype()
{
	m_iTag = (_uint)TAG_ENEMY;
	m_eExecutionTag = ODUR;
	return S_OK;
}

HRESULT COdur::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	if (FAILED(Ready_Weapons()))
		return E_FAIL;

	if (FAILED(Ready_Stats()))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	m_pSwapBone = m_pModel->Get_Bone("weapon_l_Sword");
	Safe_AddRef(m_pSwapBone);

	Change_State((_uint)OdurState::State_Idle);
	m_pModel->Set_AnimPlay();

	m_tEffectSpawnDesc.pParentTransform = m_pTransform;
	m_tEffectSpawnDesc.pParentModel = m_pModel;

	CUTSCENEMGR->Add_Actor(this);

	m_bNoRender = true;
	m_bLookTarget = false;

	m_iStunnedStateIdx = (_uint)OdurState::State_Stunned_Loop;
	m_iExecutionStateIdx = (_uint)OdurState::State_Executed;

	Bind_KeyFrames();
	Bind_KeyFrameEffects();

	return S_OK;
}

void COdur::Tick(_float fTimeDelta)
{
	if (KEY_DOWN(eKeyCode::T))
	{
		m_bNoRender = false;
		UIMGR->Active_UI("UI_BossBar", m_pStats);
		m_pLightObject->Set_Active(true);
		Change_State((_uint)OdurState::State_Walk);
	}

	if (KEY_DOWN(eKeyCode::O))
	{
		//Change_State((_uint)OdurState::State_DisappearMove);
		//Change_State((_uint)OdurState::State_Parry);
		//Change_State((_uint)OdurState::State_ThrowCard);
		//Change_State((_uint)OdurState::State_ExecutionDisappear);
		Change_State((_uint)OdurState::State_Stunned_Start);
	}

	__super::Tick(fTimeDelta);

	if (m_pLightObject)
		m_pLightObject->Set_LightPosition(m_pTransform->Get_Position() + XMVectorSet(0.f, 2.f, 0.f, 0.f));

}

void COdur::LateTick(_float fTimeDelta)
{
	m_States[m_iState]->Late_Update(fTimeDelta);

	Update_Alpha(fTimeDelta);

	__super::LateTick_Weapons(fTimeDelta);

	if (m_bNoRender)
		return;

#ifdef _DEBUG
	m_pGameInstance->Add_RenderComponent(m_pCollider);
	m_pGameInstance->Add_RenderComponent(m_pHitBoxCollider);
#endif

	CRenderer::RENDERGROUP RenderGroup = m_fAlpha < m_fAlphaCritical ? CRenderer::RENDER_BLEND : CRenderer::RENDER_NONBLEND;

	m_pGameInstance->Add_RenderObject(RenderGroup, this);
	if (m_bDissolve || m_bRimLight)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_GLOW, this);
}

HRESULT COdur::Render()
{
	_uint iPassIdx = m_iPassIdx;

	if (m_fAlpha < m_fAlphaCritical && 0 == m_iPassIdx)
	{
		if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
			return E_FAIL;

		iPassIdx = 1;
	}

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		m_pModel->SetUp_OnShader(m_pShader, i, TextureType_NORMALS, "g_NormalTexture");

		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, iPassIdx)))
			return E_FAIL;  
	}

	return S_OK;
}

void COdur::OnEnter_Cutscene()
{
}

void COdur::OnStart_Cutscene(CUTSCENE_NUMBER eCutsceneNumber)
{
	Change_State((_uint)OdurState::State_Cutscene);
}

void COdur::OnEnd_Cutscene()
{
	UIMGR->Active_UI("UI_BossBar", m_pStats);
}

void COdur::Bind_KeyFrames()
{
	m_pModel->Bind_Func("Active_Odur_Cane_Collider", bind(&CWeapon::Set_Active_Collider, m_Weapons[CANE], true));
	m_pModel->Bind_Func("Inactive_Odur_Cane_Collider", bind(&CWeapon::Set_Active_Collider, m_Weapons[CANE], false));
	m_pModel->Bind_Func("Active_Odur_Sword_Collider", bind(&CWeapon::Set_Active_Collider, m_Weapons[SWORD], true));
	m_pModel->Bind_Func("Inactive_Odur_Sword_Collider", bind(&CWeapon::Set_Active_Collider, m_Weapons[SWORD], false));
	m_pModel->Bind_Func("Active_Odur_Foot_L_Collider", bind(&CWeapon::Set_Active_Collider, m_Weapons[FOOT_L], true));
	m_pModel->Bind_Func("Inactive_Odur_Foot_L_Collider", bind(&CWeapon::Set_Active_Collider, m_Weapons[FOOT_L], false));
	m_pModel->Bind_Func("Active_Odur_Foot_R_Collider", bind(&CWeapon::Set_Active_Collider, m_Weapons[FOOT_R], true));
	m_pModel->Bind_Func("Inactive_Odur_Foot_R_Collider", bind(&CWeapon::Set_Active_Collider, m_Weapons[FOOT_R], false));
	m_pModel->Bind_Func("Swap_Bone", bind(&COdur::Swap_Bone, this));
	m_pModel->Bind_Func("Enable_LookTarget", bind(&CEnemy::Set_LookTarget, this, true));
	m_pModel->Bind_Func("Disable_LookTarget", bind(&CEnemy::Set_LookTarget, this, false));
	m_pModel->Bind_Func("Enable_Stanced", bind(&CCharacter::Set_Stanced, this, true));
	m_pModel->Bind_Func("Disable_Stanced", bind(&CCharacter::Set_Stanced, this, false));
	m_pModel->Bind_Func("Update_AttackDesc", bind(&CCharacter::Update_AttackDesc, this));
	m_pModel->Bind_Func("Enable_Render", bind(&CGameObject::Set_NoRender, this, false));	
	m_pModel->Bind_Func("Odur_Execute_SlowTime", bind(&CGameInstance::Set_TimeScale, m_pGameInstance, 0.2f));	
	m_pModel->Bind_Func("Reset_Timer", bind(&CGameInstance::Set_TimeScale, m_pGameInstance, 1.f));
	m_pModel->Bind_Func("Enable_CaneTrail", bind(&CWeapon::Set_Active_Trail, m_Weapons[CANE], true));
	m_pModel->Bind_Func("Disable_CaneTrail", bind(&CWeapon::Set_Active_Trail, m_Weapons[CANE], false));
	m_pModel->Bind_Func("Enable_SwordTrail", bind(&CWeapon::Set_Active_Trail, m_Weapons[SWORD], true));
	m_pModel->Bind_Func("Disable_SwordTrail", bind(&CWeapon::Set_Active_Trail, m_Weapons[SWORD], false));
	m_pModel->Bind_Func("Odur_Light_On", bind(&CGameObject::Set_Active, m_pLightObject, true));

}

void COdur::Bind_KeyFrameEffects()
{
	m_pModel->Bind_Func("Effect_Odur_Cane_Attack1_Cloak_Particle", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Odur_Cane_Attack1_Cloak_Particle", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_Odur_Cane_Attack1_Particle", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Odur_Cane_Attack1_Particle", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_Odur_Cane_Kick_Effect", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Odur_Cane_Kick_Effect", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_Odur_Cane_Kick_Particle", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Odur_Cane_Kick_Particle", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_Odur_Disappear_Particle", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Odur_Disappear_Particle", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_Odur_Execution_Blood", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Odur_Execution_Blood", &m_tEffectSpawnDesc));
}

void COdur::Swap_Bone()
{
	m_Weapons[SWORD]->Swap_SocketBone(m_pSwapBone);
}

void COdur::Set_Alpha_Increase()
{
	m_bAlphaIncrease = true;
	m_bAlphaEnabled = true;
	for (auto& pWeapon : m_Weapons)
		pWeapon->Set_AlphaEnable(true);
}

void COdur::Set_Alpha_Decrease()
{
	m_bAlphaIncrease = false;
	m_bAlphaEnabled = true;
	for (auto& pWeapon : m_Weapons)
		pWeapon->Set_AlphaEnable(false);

}

void COdur::SetState_Death()
{
	m_bExecutionEnd = true;
}

void COdur::SetState_Executed(void* pArg)
{
	Change_State(m_iExecutionStateIdx, pArg);
	s_pTarget->Get_Transform()->Set_WorldMatrix(XMLoadFloat4x4(&m_InitWorldMatrix));

	UIMGR->Inactive_UI("UI_BossBar");
}

void COdur::Update_Alpha(_float fTimeDelta)
{
	if (false == m_bAlphaEnabled)
		return;
		
	_float fDeltaSpeed = m_fDeltaAlphaSpeed * (m_bAlphaIncrease == false ? -1.f : 1.f);

	m_fAlpha += fDeltaSpeed * fTimeDelta;

	m_fAlpha = clamp(m_fAlpha, 0.f, 1.f);

	if (1.f == m_fAlpha || 0.f == m_fAlpha)
		m_bAlphaEnabled = false;

	Update_WeaponAlpha();
}

void COdur::Update_WeaponAlpha()
{
	for (auto pWeapon : m_Weapons)
		pWeapon->Set_Alpha(m_fAlpha);
}

HRESULT COdur::Ready_Components(void* pArg)
{
	LOADOBJDESC* pLoadDesc = (LOADOBJDESC*)pArg;

	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Model_Odur"), TEXT("Model"), (CComponent**)&m_pModel)))
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

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Navigation"), TEXT("Navigation"), (CComponent**)&m_pNavigation, &(pLoadDesc->iNaviIdx))))
		return E_FAIL;

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pLoadDesc->WorldMatrix));

	m_InitWorldMatrix = pLoadDesc->WorldMatrix;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Dissolve"), TEXT("Dissolve_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT COdur::Ready_States()
{
	m_States.resize((_uint)OdurState::State_End);

	m_States[(_uint)OdurState::State_Idle] = COdurState_Idle::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_Walk] = COdurState_Walk::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_Hit] = COdurState_Hit::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_CaneAttack1] = COdurState_CaneAttack1::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_CaneAttack2] = COdurState_CaneAttack2::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_KickCombo] = COdurState_KickCombo::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_Parry] = COdurState_Parry::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_DisappearWalk] = COdurState_DisappearWalk::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_DisappearMove] = COdurState_DisappearMove::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_Appear] = COdurState_Appear::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_ThrowCard] = COdurState_ThrowCard::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_ExecutionDisappear] = COdurState_ExecutionDisappear::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_ReadyExecution] = COdurState_ReadyExecution::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_Stunned_Start] = COdurState_Stunned_Start::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_Stunned_Loop] = COdurState_Stunned_Loop::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_Execute] = COdurState_Execute::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_Executed] = COdurState_Executed::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_Cutscene] = COdurState_Cutscene::Create(m_pDevice, m_pContext, this);

	return S_OK;
}

HRESULT COdur::Ready_Weapons()
{
	m_Weapons.resize(WEAPON_END);

	CCollider::COLLIDERDESC ColliderDesc = {};
	ColliderDesc.eType = CCollider::OBB;
	ColliderDesc.pOwner = this;
	ColliderDesc.vCenter = { 0.2f, 0.f, 0.f };
	ColliderDesc.vSize = { 1.f, 0.1f, 0.1f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };
	ColliderDesc.bActive = false;
	ColliderDesc.strCollisionLayer = "Enemy_Weapon";

	CWeapon::WEAPONDESC WeaponDesc;
	WeaponDesc.iTag = (_uint)TAG_ENEMY_WEAPON;
	WeaponDesc.iLevelID = LEVEL_ODUR;
	WeaponDesc.pParentTransform = m_pTransform;
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_Cane");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Odur_Cane";
	WeaponDesc.pOwner = this;
	WeaponDesc.pColliderDesc = &ColliderDesc;
	WeaponDesc.bAlphaBlend = false;
	WeaponDesc.bTrail = true;
	WeaponDesc.wstrTrailTag = L"Prototype_Trail_Odur_Cane_Trail";

	m_Weapons[CANE] = static_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[CANE])
		return E_FAIL;

	ColliderDesc.eType = CCollider::OBB;
	ColliderDesc.vCenter = { 0.7f, 0.f, 0.f };
	ColliderDesc.vSize = { 1.3f, 0.1f, 0.1f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };

	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_r_Sword");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Odur_Sword";
	WeaponDesc.wstrTrailTag = L"Prototype_Trail_Odur_Sword_Trail";
	m_Weapons[SWORD] = static_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[SWORD])
		return E_FAIL;

	WeaponDesc.pSocketBone = m_pModel->Get_Bone("ball_l");
	WeaponDesc.wstrModelTag = L"";
	WeaponDesc.pColliderDesc = &ColliderDesc;

	ColliderDesc.eType = CCollider::SPHERE;
	ColliderDesc.vCenter = { 0.f, 0.f, 0.f };
	ColliderDesc.vSize = { 1.f, 0.f, 0.f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };

	m_Weapons[FOOT_L] = static_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[FOOT_L])
		return E_FAIL;

	WeaponDesc.pSocketBone = m_pModel->Get_Bone("ball_r");
	m_Weapons[FOOT_R] = static_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[FOOT_R])
		return E_FAIL;

	return S_OK;
}

HRESULT COdur::Ready_Stats()
{
	ENEMYDESC EnemyDesc;
	EnemyDesc.wstrEnemyName = L"¿ÀµÎ¸£";
	EnemyDesc.iMaxHp = 1000;
	EnemyDesc.bIsBoss = true;

	m_pStats = CEnemyStats::Create(EnemyDesc);

	return S_OK;
}

HRESULT COdur::Ready_Light()
{
	LIGHT_DESC LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vAmbient = { 0.5f, 0.5f, 0.5f, 1.f };
	LightDesc.vDiffuse = { 0.8f, 0.8f, 0.8f, 1.f };
	LightDesc.fRange = 5.f;
	LightDesc.fLightStrength = 1.f;

	m_pLightObject = static_cast<CLightObject*>(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Light", L"Prototype_LightObject", &LightDesc));
	if (nullptr == m_pLightObject)
		return E_FAIL;
	m_pLightObject->Set_LightPosition(m_pTransform->Get_Position() + XMVectorSet(0.f, 2.2f, 0.f, 0.f));
	m_pLightObject->Set_Active(false);

	return S_OK;
}



COdur* COdur::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	COdur* pInstance = new COdur(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : COdur"));
		Safe_Release(pInstance);
	}

	return pInstance;

	
}

CGameObject* COdur::Clone(void* pArg)
{
	COdur* pInstance = new COdur(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : COdur"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdur::Free()
{
	__super::Free();

	Safe_Release(m_pSwapBone);
	
}
