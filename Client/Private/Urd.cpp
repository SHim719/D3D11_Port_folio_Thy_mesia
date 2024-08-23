#include "Urd.h"
#include "Urd_States.h"
#include "Weapon.h"
#include "Bone.h"

#include "Cutscene_Manager.h"
#include "UI_Manager.h"
#include "Effect_Manager.h"

#include "FadeScreen.h"

#include "LightObject.h"

#include "Player.h"

#include "Urd_Weapon.h"
#include "Urd_MagicCircle.h"

#include "BossMusic_Player.h"

CUrd::CUrd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CUrd::CUrd(const CUrd& rhs)
	: CEnemy(rhs)
{
}

HRESULT CUrd::Initialize_Prototype()
{
	m_iTag = (_uint)TAG_ENEMY;
	m_eExecutionTag = URD;
	return S_OK;
}

HRESULT CUrd::Initialize(void* pArg)
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

	m_pSwapBone = m_pModel->Get_Bone("weapon_r");
	Safe_AddRef(m_pSwapBone);

	Change_State((_uint)UrdState::State_Idle);
	m_pModel->Set_AnimPlay();

	m_tEffectSpawnDesc.pParentTransform = m_pTransform;
	m_tEffectSpawnDesc.pParentModel = m_pModel;

	CUTSCENEMGR->Add_Actor(this);

	m_bNoRender = true;
	m_bLookTarget = false;

	m_iStunnedStateIdx = (_uint)UrdState::State_Stunned_Loop;
	m_iExecutionStateIdx = (_uint)UrdState::State_Executed;

	m_pMagicCircle = static_cast<CUrd_MagicCircle*>(m_pGameInstance->Clone_GameObject(L"Prototype_Urd_MagicCircle"));
	m_pMagicCircle->Set_ForUltimate();

	Bind_KeyFrames();
	Bind_KeyFrameEffects();
	Bind_KeyFrameSounds();

	return S_OK;
}

void CUrd::Tick(_float fTimeDelta)
{
	if (KEY_DOWN(eKeyCode::T))
	{
		m_bNoRender = false;
		UIMGR->Active_UI("UI_BossBar", m_pStats);
		m_pLightObject->Set_Active(true);
		Change_State((_uint)UrdState::State_Walk);
		//Active_Phase2();
		Play_BGM();
	}
	
	if (KEY_DOWN(eKeyCode::O))
	{
		//Change_State((_uint)UrdState::State_Skill1);
		//Change_State((_uint)UrdState::State_Skill3);
		Change_State((_uint)UrdState::State_Ultimate);
		//Change_State((_uint)UrdState::State_TripleStab);
		//Change_State((_uint)UrdState::State_Stunned_Start);
		//Change_State((_uint)UrdState::State_DefaultAttack);
	}

	if (KEY_DOWN(eKeyCode::I))
	{
		Change_State((_uint)UrdState::State_Parry);
		//Change_State((_uint)UrdState::State_Skill3);
		//Change_State((_uint)UrdState::State_Ultimate);
		//Change_State((_uint)UrdState::State_Stunned_Start);
	}


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

	if (m_bDissolve)
	{
		Update_Dissolve(fTimeDelta);
		if (false == m_bDissolve)
		{
			Set_Active(false);
			m_Weapons[SWORD]->Set_Active(false);
		}
	}

	if (m_bRimLight)
	{
		Update_RimLight(fTimeDelta);
	}


	if (false == m_bPauseAnim)
		m_pModel->Play_Animation(fTimeDelta);

	
	if (m_bPauseAnim)
	{
		m_fPauseTerm -= fTimeDelta;
		if (m_fPauseTerm < 0.f)
			m_bPauseAnim = false;
	}

	if (m_bSPSkill_Explosion)
		Explode_Time_Lag(fTimeDelta);
	
	if (m_pLightObject)
		m_pLightObject->Set_LightPosition(m_pTransform->Get_Position() + XMVectorSet(0.f, 2.f, 0.f, 0.f));
}

void CUrd::LateTick(_float fTimeDelta)
{
	m_States[m_iState]->Late_Update(fTimeDelta);

	__super::LateTick_Weapons(fTimeDelta);

	if (m_bNoRender)
		return;

#ifdef _DEBUG
	m_pGameInstance->Add_RenderComponent(m_pCollider);
	m_pGameInstance->Add_RenderComponent(m_pHitBoxCollider);
#endif

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	if (m_bDissolve || m_bRimLight)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_GLOW, this);
}

HRESULT CUrd::Render()
{
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

		if (FAILED(m_pModel->Render(m_pShader, i, m_iPassIdx)))
			return E_FAIL;
	}

	return S_OK;
}

void CUrd::OnEnter_Cutscene()
{
}

void CUrd::OnStart_Cutscene(CUTSCENE_NUMBER eCutsceneNumber)
{
	Change_State((_uint)UrdState::State_Cutscene);
}

void CUrd::OnEnd_Cutscene()
{
	UIMGR->Active_UI("UI_BossBar", m_pStats);
	Play_BGM();
}

void CUrd::Play_BGM()
{
	vector<BOSSMUSICDESC> BossMusics;
	BossMusics.reserve(2);

	BOSSMUSICDESC Desc{};
	Desc.wstrMusicTag = L"BGM_Urd_Phase1";
	Desc.fLoopStartPos = 1.f;

	BossMusics.emplace_back(Desc);

	Desc.wstrMusicTag = L"BGM_Urd_Phase2";
	Desc.fLoopStartPos = 85.205f;

	BossMusics.emplace_back(Desc);

	Get_Inst(CBossMusic_Player)->Play_BossBGM(BossMusics);
}

void CUrd::Bind_KeyFrames()
{
	m_pModel->Bind_Func("Active_WeaponCollider", bind(&CWeapon::Set_Active_Collider, m_Weapons[SWORD], true));
	m_pModel->Bind_Func("Inactive_WeaponCollider", bind(&CWeapon::Set_Active_Collider, m_Weapons[SWORD], false));
	m_pModel->Bind_Func("Update_AttackDesc", bind(&CCharacter::Update_AttackDesc, this));
	m_pModel->Bind_Func("Swap_Bone", bind(&CUrd::Swap_Bone, this));
	m_pModel->Bind_Func("Enable_LookTarget", bind(&CEnemy::Set_LookTarget, this, true));
	m_pModel->Bind_Func("Disable_LookTarget", bind(&CEnemy::Set_LookTarget, this, false));
	m_pModel->Bind_Func("Enable_Stanced", bind(&CCharacter::Set_Stanced, this, true));
	m_pModel->Bind_Func("Disable_Stanced", bind(&CCharacter::Set_Stanced, this, false));
	m_pModel->Bind_Func("Enable_NaviY", bind(&CCharacter::Set_Adjust_NaviY, this, true));
	m_pModel->Bind_Func("Active_Dissolve", bind(&CCharacter::Active_Dissolve, this));
	m_pModel->Bind_Func("Update_AttackDesc", bind(&CCharacter::Update_AttackDesc, this));
	m_pModel->Bind_Func("Set_Vulnerable", bind(&CUrd::Set_Invincible, this, false));
	m_pModel->Bind_Func("Active_Trail", bind(&CWeapon::Set_Active_Trail, m_Weapons[SWORD], true));
	m_pModel->Bind_Func("Inactive_Trail", bind(&CWeapon::Set_Active_Trail, m_Weapons[SWORD], false));
	m_pModel->Bind_Func("Release_Weapon", bind(&CUrd::Release_UrdWeapon, this, false));
	m_pModel->Bind_Func("Throw_Weapon", bind(&CUrd::Release_UrdWeapon, this, true));
	m_pModel->Bind_Func("Active_MagicCircle_SP", bind(&CUrd::Active_MagicCircle, this, true));
	m_pModel->Bind_Func("Urd_Ultimate", bind(&CUrd::Active_Ultimate_Skill, this));
	m_pModel->Bind_Func("Decide_State", bind(&CUrd::Decide_State, this));

	m_pModel->Bind_Func("Pause_Anim_SP", bind(&CUrd::Pause_Anim, this, 0.8f));
	m_pModel->Bind_Func("Pause_Anim_TS", bind(&CUrd::Pause_Anim, this, 0.5f));
}

void CUrd::Bind_KeyFrameEffects()
{
	m_pModel->Bind_Func("Effect_Cutscene_Sword_Impact", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Urd_Cutscene_Sword_Impact", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_Cutscene_Particle", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Urd_Cutscene_Particle", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_Urd_Ready_SP01", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Urd_Ready_SP01", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_Urd_Sword_Default", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Urd_Sword_Default", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_Slash_Horizontal", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Urd_Slash_Horizontal", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_Slash_Vertical", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Urd_Slash_Vertical", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_Strong_Stab", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Urd_Strong_Stab", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_Strong_Stab_Particle", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Urd_Strong_Stab_Particle", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_Pierce_Start", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Urd_Pierce_Start", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_Pierce", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Urd_Pierce", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_DefaultAttack1", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Urd_DefaultAttack_Particle1", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_DefaultAttack2", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Urd_DefaultAttack_Particle2", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_DashStab", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Urd_Dash_Stab", &m_tEffectSpawnDesc));
}

void CUrd::Bind_KeyFrameSounds()
{
	m_pModel->Bind_Func("Sound_SkillStart", bind(&CGameInstance::Play, m_pGameInstance, L"Urd_SkillStart", false, 1.f));
	m_pModel->Bind_Func("Sound_Voice_Skill1_2", bind(&CGameInstance::Play, m_pGameInstance, L"Urd_Voice_Skill1_2", false, 1.f));
	m_pModel->Bind_Func("Sound_Voice_Skill3", bind(&CGameInstance::Play, m_pGameInstance, L"Urd_Voice_Skill3", false, 1.f));
	m_pModel->Bind_Func("Sound_Voice_Cutscene1", bind(&CGameInstance::Play, m_pGameInstance, L"Urd_Voice_CutScene1", false, 1.f));
	m_pModel->Bind_Func("Sound_Voice_Cutscene2", bind(&CGameInstance::Play, m_pGameInstance, L"Urd_Voice_CutScene2", false, 1.f));
	m_pModel->Bind_Func("Sound_Voice_Attack2", bind(&CGameInstance::Play, m_pGameInstance, L"Urd_Voice_Attack2", false, 1.f));
	m_pModel->Bind_Func("Sound_Urd_Attack6", bind(&CGameInstance::Play, m_pGameInstance, L"Urd_Attack6", false, 1.f));
	m_pModel->Bind_Func("Sound_Urd_Attack7", bind(&CGameInstance::Play, m_pGameInstance, L"Urd_Attack7", false, 1.f));
	m_pModel->Bind_Func("Sound_Urd_Attack2", bind(&CGameInstance::Play, m_pGameInstance, L"Urd_Attack2", false, 1.f));
	m_pModel->Bind_Func("Sound_Urd_FootStep", bind(&CGameInstance::Play_RandomSound, m_pGameInstance, L"Urd_FootStep", 1, 2, false, 1.f));
	m_pModel->Bind_Func("Sound_Voice_ExtraAttack", bind(&CGameInstance::Play_RandomSound, m_pGameInstance, L"Urd_Voice_ExtraAttack", 1, 2, false, 1.f));

}

void CUrd::Active_Ultimate_Skill()
{
	CBone* pWeaponBone = m_pModel->Get_Bone("weapon_r");
	CALC_TF->Attach_To_Bone(pWeaponBone, m_pTransform, XMMatrixTranslation(0.8f, 0.f, 0.f), true);
	_vector vEffectSpawnPos = CALC_TF->Get_Position();

	CGameObject* pSphere = m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Stencil", L"Prototype_Urd_InversionSphere");
	pSphere->Get_Transform()->Set_Position(vEffectSpawnPos);

	m_pMagicCircle->Explosion();

	m_iNowWeaponIdx = 99999;
	m_bSPSkill_Explosion = true;

	PLAY_SOUND(L"Urd_SPSKill01_Cast", false, 1.f);
	PLAY_SOUND(L"Urd_SPSKill01_Explosion", false, 1.f);
}

void CUrd::Explode_Time_Lag(_float fTimeDelta)
{
	m_fExplosion_GapAcc += fTimeDelta;
	if (m_fExplosion_GapAcc > m_fExplosion_Gap)
	{
		m_fExplosion_GapAcc = 0.f;
		for (size_t i = 0; i < m_Urd_Weapons.size(); ++i)
		{
			if (true == m_Urd_Weapons[i]->Is_Using())
			{
				m_Urd_Weapons[i]->Explode_MagicCircle(true);
				m_Urd_Weapons[i]->Disappear();
				break;
			}
				
		}
		m_iNumActivatedWeapons = m_iNumActivatedWeapons > 0 ? m_iNumActivatedWeapons - 1 : 0;
		if (0 == m_iNumActivatedWeapons)
			m_bSPSkill_Explosion = false;
		
			
	} 
}

void CUrd::Swap_Bone()
{
	m_Weapons[SWORD]->Swap_SocketBone(m_pSwapBone);
}

void CUrd::Active_UrdWeapon()
{
	for (size_t i = 0; i < m_Urd_Weapons.size(); ++i)
	{
		if (false == m_Urd_Weapons[i]->Is_Using())
		{
			Safe_AddRef(m_Urd_Weapons[i]);
			m_iNowWeaponIdx = i;
			m_Urd_Weapons[i]->OnEnter_Layer(nullptr);
			m_pGameInstance->Insert_GameObject(GET_CURLEVEL, L"Enemy_Weapon", m_Urd_Weapons[i]);
			++m_iNumActivatedWeapons;
			return;
		}
	}
}

void CUrd::Active_MagicCircle(_bool bUltimate)
{
	if (bUltimate)
	{
		Safe_AddRef(m_pMagicCircle);
		CUrd_MagicCircle::URD_MAGICCIRCLE_DESC Desc;
		_vector vSpawnPos = m_pTransform->Get_Position();
		vSpawnPos = XMVectorSetY(vSpawnPos, 0.3f);
		XMStoreFloat4(&Desc.vPosition, vSpawnPos);
		Desc.bUltimate = true;

		m_pMagicCircle->OnEnter_Layer(&Desc);
		m_pGameInstance->Insert_GameObject(GET_CURLEVEL, L"Effect", m_pMagicCircle);
		m_iNowWeaponIdx = 99999;
	}

	for (size_t i = 0; i < m_Urd_Weapons.size(); ++i)
	{
		if (true == m_Urd_Weapons[i]->Is_Using() && i != m_iNowWeaponIdx)
			m_Urd_Weapons[i]->Active_MagicCircle(bUltimate);
	}

	m_iNowWeaponIdx = 99999;
}


void CUrd::Release_UrdWeapon(_bool bThrow)
{
	m_Urd_Weapons[m_iNowWeaponIdx]->Released(bThrow);
}

void CUrd::SetState_Death()
{
	m_bExecutionEnd = true;
}

void CUrd::SetState_Executed(void* pArg)
{
	if (true == m_bPhase2)
		UIMGR->Inactive_UI("UI_BossBar");

	Change_State(m_iExecutionStateIdx, pArg);
	s_pTarget->Get_Transform()->Set_WorldMatrix(XMLoadFloat4x4(&m_InitWorldMatrix));
	CNavigation* pNavigation = static_cast<CNavigation*>(s_pTarget->Find_Component(L"Navigation"));

	_fvector vPosition = XMLoadFloat4x4(&m_InitWorldMatrix).r[3];

	pNavigation->Set_CurrentIdx(vPosition);

	static_cast<CCharacter*>(s_pTarget)->Compute_YPos();

	m_Weapons[SWORD]->Set_Active_Trail(false);
}

void CUrd::Decide_State()
{
	static_cast<CUrdState_Base*>(m_States[m_iState])->Decide_State();
}

void CUrd::Active_Phase2()
{
	m_bPhase2 = true;
	m_pStats->Increase_Hp(m_pStats->Get_MaxHp());
	Set_Active_Colliders(true);
}

void CUrd::Resize_WeaponCollider(_bool bOrigin)
{
	CCollider* pWeaponCollider = static_cast<CCollider*>(m_Weapons[SWORD]->Find_Component(L"Collider"));

	if (bOrigin)
	{
		pWeaponCollider->Set_Center(XMVectorSet(0.5f, 0.f, 0.f, 1.f));
		pWeaponCollider->Set_Size(XMVectorSet(1.2f, 0.1f, 0.1f, 1.f));	
	}
	else
	{
		pWeaponCollider->Set_Center(XMVectorSet(1.7f, 0.f, 0.f, 1.f));
		pWeaponCollider->Set_Size(XMVectorSet(3.f, 0.1f, 0.1f, 1.f));
	}
	
}

void CUrd::Set_NowWeapon_Disappear()
{
	if (m_iNowWeaponIdx >= m_Urd_Weapons.size())
		return;

	m_Urd_Weapons[m_iNowWeaponIdx]->Disappear();

}

HRESULT CUrd::Ready_Components(void* pArg)
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

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Model_Urd"), TEXT("Model"), (CComponent**)&m_pModel)))
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

HRESULT CUrd::Ready_States()
{
	m_States.resize((_uint)UrdState::State_End);

	m_States[(_uint)UrdState::State_Idle] = CUrdState_Idle::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_Walk] = CUrdState_Walk::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_Step_F] = CUrdState_Step_F::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_Step_B] = CUrdState_Step_B::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_Step_FLR] = CUrdState_Step_FLR::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_Step_LR] = CUrdState_Step_LR::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_DefaultAttack] = CUrdState_DefaultAttack::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_DashStab] = CUrdState_DashStab::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_ExtraAttack] = CUrdState_ExtraAttack::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_StrongStab] = CUrdState_StrongStab::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_TripleStab] = CUrdState_TripleStab::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_Skill1] = CUrdState_Skill1::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_Skill2] = CUrdState_Skill2::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_Skill3] = CUrdState_Skill3::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_Ultimate] = CUrdState_Ultimate::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_Hit] = CUrdState_Hit::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_Parry] = CUrdState_Parry::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_Stunned_Start] = CUrdState_Stunned_Start::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_Stunned_Loop] = CUrdState_Stunned_Loop::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_Executed] = CUrdState_Executed::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_Dead] = CUrdState_Dead::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)UrdState::State_Cutscene] = CUrdState_Cutscene::Create(m_pDevice, m_pContext, this);

	return S_OK;
}

HRESULT CUrd::Ready_Weapons()
{
	m_Weapons.resize(WEAPON_END);

	CCollider::COLLIDERDESC ColliderDesc = {};
	ColliderDesc.eType = CCollider::OBB;
	ColliderDesc.pOwner = this;
	ColliderDesc.vCenter = { 0.5f, 0.f, 0.f };
	ColliderDesc.vSize = { 1.2f, 0.1f, 0.1f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };
	ColliderDesc.bActive = false;
	ColliderDesc.strCollisionLayer = "Enemy_Weapon";
	
	CWeapon::WEAPONDESC WeaponDesc;
	WeaponDesc.iTag = (_uint)TAG_ENEMY_WEAPON;
	WeaponDesc.iLevelID = LEVEL_URD;
	WeaponDesc.pParentTransform = m_pTransform;
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("AnimTargetPoint");//m_pModel->Get_Bone("weapon_r");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Urd_Sword";
	WeaponDesc.pOwner = this;
	WeaponDesc.pColliderDesc = &ColliderDesc;
	WeaponDesc.bAlphaBlend = false;
	WeaponDesc.bTrail = true;
	WeaponDesc.wstrTrailTag = L"Prototype_Trail_Urd_Sword_Trail";
	
	m_Weapons[SWORD] = static_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[SWORD])
		return E_FAIL;

	ColliderDesc.eType = CCollider::SPHERE;
	ColliderDesc.pOwner = this;
	ColliderDesc.vCenter = { 0.f, 0.f, 0.f };
	ColliderDesc.vSize = { 1.5f, 0.1f, 0.1f };

	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_l");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Urd_Weapon_VFX";
	WeaponDesc.pOwner = this;
	WeaponDesc.bAlphaBlend = false;


	m_Urd_Weapons.resize(9);
	for (_uint i = 0; i < 9; ++i)
	{
		m_Urd_Weapons[i] = static_cast<CUrd_Weapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_Urd_Weapon", &WeaponDesc));
		if (nullptr == m_Urd_Weapons[i])
			assert(false);
	}

	return S_OK;
}

HRESULT CUrd::Ready_Stats()
{
	ENEMYDESC EnemyDesc;
	EnemyDesc.wstrEnemyName = L"¿ì¸£µå";
	EnemyDesc.iMaxHp = 300;
	EnemyDesc.bIsBoss = true;

	m_pStats = CEnemyStats::Create(EnemyDesc);

	return S_OK;
}

HRESULT CUrd::Ready_Light()
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



CUrd* CUrd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUrd* pInstance = new CUrd(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUrd"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUrd::Clone(void* pArg)
{
	CUrd* pInstance = new CUrd(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUrd"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrd::Free()
{
	__super::Free();

	Safe_Release(m_pSwapBone);
	Safe_Release(m_pMagicCircle);

	for (auto& pUrd_Weapon : m_Urd_Weapons)
		Safe_Release(pUrd_Weapon);
}
