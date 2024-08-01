#include "Player.h"

#include "Player_States.h"
#include "PlayerStats.h"

#include "PlagueWeapon.h"

#include "Enemy.h"

#include "Cutscene_Manager.h"
#include "UI_Manager.h"
#include "UI_DamageFont.h"

#include "Bone.h"

#include "Ladder_Down.h"
#include "Ladder_Up.h"

#include "FadeScreen.h"

#include "Effect_Manager.h"
#include "GameEffect.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CCharacter(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	m_iTag = (_uint)TAG_PLAYER;
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	if (FAILED(Ready_Weapons()))
		return E_FAIL;

	if (FAILED(Ready_PlagueWeapons()))
		return E_FAIL;

	Bind_KeyFrames();
	Bind_KeyFrameEffects();

	m_tEffectSpawnDesc.pParentTransform = m_pTransform;
	m_tEffectSpawnDesc.pParentModel = m_pModel;

	CEnemy::Set_Target(this);

	m_pTransform->Set_MoveLook(m_pTransform->Get_Look());

	m_pModel->Set_AnimPlay();

	Change_State((_uint)PlayerState::State_Idle);
	m_pModel->Change_Animation(Corvus_SD_Idle, 0.f);

	CUTSCENEMGR->Add_Actor(this);

	return S_OK;
}

void CPlayer::PriorityTick(_float fTimeDelta)
{
	Calc_HitGap(fTimeDelta);
	Update_CanExecutionEnemy();
}

void CPlayer::Tick(_float fTimeDelta)
{
	if (KEY_DOWN(eKeyCode::V))
	{
		//m_pStats->Active_Skill(SKILLTYPE::AXE);
		//m_pStats->Obtain_Key();
		//
		//vector<SKILLTYPE> eTypes{ SKILLTYPE::AXE, SKILLTYPE::NONE };
		//
		//UIMGR->Active_UI("UI_Popup", &eTypes);

		m_pStats->Update_PlunderSkill(SKILLTYPE::HAMMER);
		//m_pStats->Update_PlunderSkill(SKILLTYPE::SPEAR);
		//m_pStats->Update_PlunderSkill(SKILLTYPE::TWINBLADE);
	}

	if (KEY_DOWN(eKeyCode::M))
		Change_State((_uint)PlayerState::State_Healing);

	if (KEY_DOWN(eKeyCode::N))
		Change_State((_uint)PlayerState::State_ParrySuccess);
	

	if (m_bLockOn)
	{
		m_pTransform->Rotation_Quaternion(
			JoMath::Slerp_TargetLook(m_pTransform->Get_GroundLook()
				, JoMath::Calc_GroundLook(m_pTargetTransform->Get_Position(), m_pTransform->Get_Position())
				, 10.f * fTimeDelta));
	}
		

	m_States[m_iState]->Update(fTimeDelta);

	if (m_bAdjustNaviY)
		Compute_YPos();

	__super::Update_Colliders();

	__super::Tick_Weapons(fTimeDelta);

	m_pModel->Play_Animation(fTimeDelta);
}

void CPlayer::LateTick(_float fTimeDelta)
{
	m_States[m_iState]->Late_Update(fTimeDelta);

	__super::LateTick_Weapons(fTimeDelta);

	if (m_bNoRender)
		return;

#ifdef _DEBUG
	m_pGameInstance->Add_RenderComponent(m_pCollider);
	m_pGameInstance->Add_RenderComponent(m_pHitBoxCollider);
	m_pGameInstance->Add_RenderComponent(m_pNavigation);
#endif

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_GLOW, this);
}

HRESULT CPlayer::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	m_pModel->SetUp_BoneMatrices(m_pShader);

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModel->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/

		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, m_iPassIdx)))
			return E_FAIL;
	}

	return S_OK;
}

void CPlayer::OnEnter_Cutscene()
{
	XMStoreFloat4x4(&m_PrevWorldMatrix, m_pTransform->Get_WorldMatrix());
	Change_State((_uint)PlayerState::State_Idle);
}

void CPlayer::OnStart_Cutscene(CUTSCENE_NUMBER eCutsceneNumber)
{
	Change_State((_uint)PlayerState::State_Cutscene, &eCutsceneNumber);

	UIMGR->Inactive_UI("UI_PlayerDefault");
}

void CPlayer::OnEnd_Cutscene()
{
	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&m_PrevWorldMatrix));

	UIMGR->Active_UI("UI_PlayerDefault");

	Change_State((_uint)PlayerState::State_Idle);
}

void CPlayer::Bind_KeyFrames()
{
	m_pModel->Bind_Func("Active_SaberCollider", bind(&CWeapon::Set_Active_Collider, m_Weapons[SABER], true));
	m_pModel->Bind_Func("Inactive_SaberCollider", bind(&CWeapon::Set_Active_Collider, m_Weapons[SABER], false));
	m_pModel->Bind_Func("Active_Claw_R", bind(&CWeapon::Set_Active_Collider, m_Weapons[CLAW_R], true));
	m_pModel->Bind_Func("Inactive_Claw_R", bind(&CWeapon::Set_Active_Collider, m_Weapons[CLAW_R], false));
	m_pModel->Bind_Func("Active_Claw_L", bind(&CWeapon::Set_Active_Collider, m_Weapons[CLAW_L], true));
	m_pModel->Bind_Func("Inactive_Claw_L", bind(&CWeapon::Set_Active_Collider, m_Weapons[CLAW_L], false));
	m_pModel->Bind_Func("Enable_NextState", bind(&CPlayer::Set_CanNextState, this, true));
	m_pModel->Bind_Func("Disable_NextState", bind(&CPlayer::Set_CanNextState, this, false));
	m_pModel->Bind_Func("Disable_Rotation", bind(&CPlayer::Set_CanRotation, this, false));
	m_pModel->Bind_Func("Enable_NextAttack", bind(&CPlayer::Set_CanNextAttack, this, true));
	m_pModel->Bind_Func("Disable_NextAttack", bind(&CPlayer::Set_CanNextAttack, this, false));
	m_pModel->Bind_Func("Set_Vulnerable", bind(&CPlayer::Set_Invincible, this, false));
	m_pModel->Bind_Func("Enable_Render", bind(&CGameObject::Set_NoRender, this, false));
	m_pModel->Bind_Func("Active_PlagueWeapon_Collider", bind(&CPlayer::Set_Active_NowPWCollider, this, true));
	m_pModel->Bind_Func("Inactive_PlagueWeapon_Collider", bind(&CPlayer::Set_Active_NowPWCollider, this, false));
	m_pModel->Bind_Func("Enable_NaviY", bind(&CPlayer::Set_Adjust_NaviY, this, true));
	m_pModel->Bind_Func("Disable_NaviY", bind(&CPlayer::Set_Adjust_NaviY, this, false));
	m_pModel->Bind_Func("ChangeToNextComboAnim", bind(&CPlayer::ChangeToNextComboAnim, this));
	m_pModel->Bind_Func("Active_PW_Twin_L_Collider", bind(&CPlayer::Set_Active_WeaponCollider, this, PW_TWINBLADE_L, true));
	m_pModel->Bind_Func("Inactive_PW_Twin_L_Collider", bind(&CPlayer::Set_Active_WeaponCollider, this, PW_TWINBLADE_L, false));
	m_pModel->Bind_Func("Active_PW_Twin_R_Collider", bind(&CPlayer::Set_Active_WeaponCollider, this, PW_TWINBLADE_R, true));
	m_pModel->Bind_Func("Inactive_PW_Twin_R_Collider", bind(&CPlayer::Set_Active_WeaponCollider, this, PW_TWINBLADE_R, false));
	m_pModel->Bind_Func("Healing", bind(&CPlayer::Healing, this));

}

void CPlayer::Bind_KeyFrameEffects()
{
	m_pModel->Bind_Func("Effect_LAttack1", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_LAttack1", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_LAttack2", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_LAttack2", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_LAttack3", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_LAttack3", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_LAttack4", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_LAttack4", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_Claw_Long1", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_Claw_Long1", &m_tEffectSpawnDesc));
	//m_pModel->Bind_Func("Effect_Claw_Long2", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_Claw_Long2", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_PW_Axe_StartParticle", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_PW_Axe_StartParticle", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_PW_Axe_Slash", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_PW_Axe_Slash", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_PW_Axe_Impact", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_PW_Axe_Impact", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_PW_Spear_Start", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_PW_Spear_Start", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_PW_Spear_Particle", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_PW_Spear_Particle", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_PW_Spear_Slash", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_PW_Spear_Slash", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_PW_Hammer_Start", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_PW_Hammer_Start", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_PW_Hammer_Slash", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_PW_Hammer_Start", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_PW_Hammer_Impact", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_PW_Hammer_Impact", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_PW_TwinBlade_Start", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_PW_TwinBlade_Start", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_PW_TwinBlade_Slash0", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_PW_TwinBlade_Slash0", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_PW_TwinBlade_Slash1", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_PW_TwinBlade_Slash1", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_PW_TwinBlade_Slash2", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_PW_TwinBlade_Slash2", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_PW_TwinBlade_Slash3", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_PW_TwinBlade_Slash3", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_PW_TwinBlade_Slash4", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_PW_TwinBlade_Slash4", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_PW_TwinBlade_Final", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_PW_TwinBlade_Slash_End", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_Plunder_Rush_Trail", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_Plunder_Rush_Trail", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_Plunder_Start", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_Plunder_Start", &m_tEffectSpawnDesc));
	m_pModel->Bind_Func("Effect_Plunder", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, "Effect_Corvus_Plunder_Effect", &m_tEffectSpawnDesc));
}

void CPlayer::Update_CanExecutionEnemy()
{
	m_pExecutionTarget = nullptr;

	if (m_bLockOn)
	{
		_float fAngle = JoMath::Calc_AngleToTarget(m_pTransform->Get_Position(), m_pTargetTransform->Get_Position(), m_pTransform->Get_GroundLook());
		if (fabsf(fAngle) < To_Radian(60.f))
		{
			_float fDist = XMVector3Length(m_pTransform->Get_Position() - m_pTargetTransform->Get_Position()).m128_f32[0];
			if (fDist < m_fExecutionDist)
			{
				for (auto it = m_StunnedEnemies.begin(); it != m_StunnedEnemies.end(); )
				{
					if ((*it)->Is_Stunned())
					{
						if ((*it)->Get_Transform() == m_pTargetTransform)
						{
							m_pExecutionTarget = *it;
							m_pExecutionTarget->Active_StunnedMark();
						}
						else
						{
							m_pExecutionTarget->InActive_StunnedMark();
						}
						++it;
					}
					else
					{
						it = m_StunnedEnemies.erase(it);
					}

					
				}
			}
		}
	}
	else
	{
		_float fMaxDist = m_fExecutionDist;

		for (auto it = m_StunnedEnemies.begin(); it != m_StunnedEnemies.end(); )
		{
			if ((*it)->Is_Stunned())
			{
				_float fAngle = JoMath::Calc_AngleToTarget(m_pTransform->Get_Position(), (*it)->Get_Transform()->Get_Position(), m_pTransform->Get_GroundLook());
				if (fabsf(fAngle) < To_Radian(60.f))
				{
					_float fDist = XMVector3Length(m_pTransform->Get_Position() - (*it)->Get_Transform()->Get_Position()).m128_f32[0];
					if (fDist < m_fExecutionDist && fDist < fMaxDist)
					{
						fMaxDist = fDist;
						m_pExecutionTarget = *it;
						m_pExecutionTarget->Active_StunnedMark();
					}
					else
					{
						(*it)->InActive_StunnedMark();
					}

				}
				else
				{
					(*it)->InActive_StunnedMark();
				}
				++it;
			}
			else
			{
				it = m_StunnedEnemies.erase(it);
			}
			
		}
	}

}


void CPlayer::Inactive_AllWeaponColliders()
{
	for (auto pWeapon : m_Weapons)
	{
		if (pWeapon)
			pWeapon->Set_Active_Collider(false);
	}
}


void CPlayer::Toggle_LockOn(CTransform* pTargetTransform)
{
	m_bLockOn = !m_bLockOn;

	Safe_Release(m_pTargetTransform);

	m_pTargetTransform = pTargetTransform;

	Safe_AddRef(m_pTargetTransform);
}

void CPlayer::SetState_Parried()
{
	Change_State((_uint)PlayerState::State_Parried);
}

void CPlayer::SetState_Executed(void* pArg)
{
	Change_State((_uint)PlayerState::State_Executed, pArg);
}

void CPlayer::SetState_Plunder(void* pArg)
{
	Change_State((_uint)PlayerState::State_Plunder, pArg);
}

void CPlayer::SetState_ClimbStart(void* pArg)
{
	if (static_cast<CPlayerState_Base*>(m_States[m_iState])->Can_Climb())
	{
		LADDERDESC* pLadderDesc = (LADDERDESC*)pArg;

		_float4* vLadderPos = (_float4*)&pLadderDesc->pWorldMatrix->m[3];
		
		_vector vPosition = XMLoadFloat4(vLadderPos) + XMLoadFloat4(&pLadderDesc->vOffset);
		m_pTransform->Set_Position(vPosition);

		// m_pTransform->LookAt2D(XMLoadFloat4(vLadderPos));
		m_pTransform->LookTo(XMLoadFloat4((_float4*)&pLadderDesc->pWorldMatrix->m[2]));

		m_iTargetNaviIdx = pLadderDesc->iTargetNaviIdx;
		m_iNowLadderHeight = pLadderDesc->iLadderHeight;
		m_iPlayerLadderHeight = pLadderDesc->iStartHeight;

		m_bIsClimbStartDown = m_iPlayerLadderHeight == 1;

		Change_State((_uint)PlayerState::State_Climb_Start, &pLadderDesc->iPlayerAnimIdx);
	}
}

void CPlayer::SetState_FoundBeacon(void* pArg)
{
	if ((_uint)PlayerState::State_FoundBeacon == m_iState || (_uint)PlayerState::State_Sit == m_iState)
		return;

	Change_State((_uint)PlayerState::State_FoundBeacon, pArg);
}

void CPlayer::SetState_Sit(void* pArg)
{
	if ((_uint)PlayerState::State_FoundBeacon == m_iState || (_uint)PlayerState::State_Sit == m_iState)
		return;

	CFadeScreen::FADEDESC FadeDesc = {};
	FadeDesc.eFadeColor = CFadeScreen::BLACK;
	FadeDesc.fFadeOutSpeed = 1.f;
	FadeDesc.fFadeInSpeed = 1.f;
	FadeDesc.pCallback_FadeOutEnd = bind(&CUI_Manager::Active_Menu, CUI_Manager::Get_Instance());

	UIMGR->Active_UI("FadeScreen", &FadeDesc);

	Change_State((_uint)PlayerState::State_Sit, pArg);
}


void CPlayer::Set_Active_DefaultWeapons(_bool bActive)
{
	m_Weapons[SABER]->Set_Active(bActive); 
	m_Weapons[DAGGER]->Set_Active(bActive);
}

void CPlayer::Set_Active_Claws(_bool bActive)
{
	m_Weapons[CLAW_L]->Set_Active(bActive); 
	m_Weapons[CLAW_R]->Set_Active(bActive);
}

void CPlayer::Set_Active_WeaponCollider(PLAYER_WEAPON eWeapon, _bool bActive)
{
	if (m_Weapons[eWeapon])
		m_Weapons[eWeapon]->Set_Active_Collider(bActive);
}

void CPlayer::Set_Active_NowPWCollider(_bool bActive)
{
	_uint iIdx = CLAW_R + m_eNowUsingSkill;
	if (m_Weapons[iIdx])
		m_Weapons[iIdx]->Set_Active_Collider(bActive);
}

void CPlayer::OnCollisionEnter(CGameObject* pOther)
{
	if (TAG_ENEMY_WEAPON == pOther->Get_Tag())
	{
		if (m_fHitGapAcc > 0.f)
			return;

		m_fHitGapAcc = m_fHitGap;
		CWeapon* pEnemyWeapon = static_cast<CWeapon*>(pOther);

		m_States[m_iState]->OnHit(pEnemyWeapon->Get_AttackDesc());
	}
	else if (TAG_ENEMY == pOther->Get_Tag())
	{
		m_bCollEnemy = true;
	}
}

void CPlayer::OnCollisionExit(CGameObject* pOther)
{
	if (TAG_ENEMY == pOther->Get_Tag())
	{
		m_bCollEnemy = false;
	}
}


void CPlayer::ChangeToNextComboAnim()
{
	m_pModel->Change_Animation(m_pModel->Get_CurrentAnimIndex() + 1);
}

void CPlayer::Healing()
{
	m_pStats->Increase_Hp(200);

	EFFECTMGR->Active_Effect("Effect_Corvus_Healing", &Get_EffectSpawnDesc());
}


_int CPlayer::Take_Damage(const ATTACKDESC& AttackDesc)
{
	CUI_DamageFont::DAMAGEFONTDESC DamageFontDesc;
	DamageFontDesc.strDamage = to_string(AttackDesc.iDamage);
	XMStoreFloat4(&DamageFontDesc.vWorldPosition, JoMath::Get_BoneWorldPos(m_pModel->Get_Bone("Bip001-Head"), m_pTransform->Get_WorldMatrix()));

	UIMGR->Active_UI("UI_DamageFont", &DamageFontDesc);

	return m_pStats->Increase_Hp(-AttackDesc.iDamage);
}

HRESULT CPlayer::Reset_NaviData(LEVEL eLevel)
{
	Safe_Release(m_pNavigation);

	CNavigation::NAVIGATION_DESC NaviDesc;

	m_pNavigation = static_cast<CNavigation*>(m_pGameInstance->Clone_Component(eLevel, L"Prototype_Navigation", &NaviDesc));
	if (nullptr == m_pNavigation)
		return E_FAIL;

	auto it = m_Components.find(L"Navigation");
	if (m_Components.end() == it)
	{
		m_Components.emplace(L"Navigation", m_pNavigation);
	}
	else
	{
		Safe_Release(it->second);
		it->second = m_pNavigation;
	}
		
	Safe_AddRef(m_pNavigation);

	m_pNavigation->Set_CurrentIdx(m_pTransform->Get_Position());
	Change_Navigation(m_pNavigation);

	return S_OK;
}

void CPlayer::Enroll_AllColliders()
{
	m_pCollider->Enroll_Collider();
	m_pHitBoxCollider->Enroll_Collider();

	for (auto pWeapon : m_Weapons)
	{
		if (nullptr != pWeapon)
			pWeapon->Enroll_Collider();
	}
}

HRESULT CPlayer::Ready_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Player"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;


	CCollider::COLLIDERDESC Desc;
	Desc.eType = CCollider::SPHERE;
	Desc.pOwner = this;
	Desc.vCenter = { 0.f, 1.f, 0.f };
	Desc.vSize = { 1.f, 0.f, 0.f };
	Desc.vRotation = { 0.f, 0.f, 0.f };
	Desc.bActive = true;
	Desc.strCollisionLayer = "Player";

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("Collider"), (CComponent**)&m_pCollider, &Desc)))
		return E_FAIL;

	Desc.eType = CCollider::SPHERE;
	Desc.pOwner = this;
	Desc.vCenter = { 0.f, 1.f, 0.f };
	Desc.vSize = { 1.f, 0.f, 0.f };
	Desc.vRotation = { 0.f, 0.f, 0.f };
	Desc.bActive = true;
	Desc.strCollisionLayer = "Player_HitBox";
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("HitBox"), (CComponent**)&m_pHitBoxCollider, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_States()
{
	m_pStats = CPlayerStats::Create();

	m_States.resize((_uint)PlayerState::State_End);

	m_States[(_uint)PlayerState::State_Idle] = CPlayerState_Idle::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Jog] = CPlayerState_Jog::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Sprint] = CPlayerState_Sprint::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Avoid] = CPlayerState_Avoid::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Attack] = CPlayerState_Attack::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_LockOn] = CPlayerState_LockOn::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Parry] = CPlayerState_Parry::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_ParrySuccess] = CPlayerState_ParrySuccess::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Hit] = CPlayerState_Hit::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_ChargeStart] = CPlayerState_ChargeStart::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_ChargeComplete] = CPlayerState_ChargeComplete::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_ChargeLoop] = CPlayerState_ChargeLoop::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_ClawAttack_Long] = CPlayerState_ClawAttack_Long::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_ClawAttack_Short] = CPlayerState_ClawAttack_Short::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_ClawAttack_ShortEnd] = CPlayerState_ClawAttack_ShortEnd::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_PlunderRush] = CPlayerState_PlunderRush::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Plunder] = CPlayerState_Plunder::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Parried] = CPlayerState_Parried::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Execution_Default] = CPlayerState_Execution_Default::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Execution_Elite] = CPlayerState_Execution_Elite::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Executed] = CPlayerState_Executed::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Climb_Start] = CPlayerState_Climb_Start::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Climb_Idle] = CPlayerState_Climb_Idle::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Climb] = CPlayerState_Climb::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Climb_End] = CPlayerState_Climb_End::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_GetUp] = CPlayerState_GetUp::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_FoundBeacon] = CPlayerState_FoundBeacon::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Sit] = CPlayerState_Sit::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Healing] = CPlayerState_Healing::Create(m_pDevice, m_pContext, this);

	m_States[(_uint)PlayerState::State_Cutscene] = CPlayerState_Cutscene::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_PW_Axe] = CPlayerState_PW_Axe::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_PW_Hammer] = CPlayerState_PW_Hammer::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_PW_Spear] = CPlayerState_PW_Spear::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_PW_TwinBlade] = CPlayerState_PW_TwinBlade::Create(m_pDevice, m_pContext, this);
	return S_OK;
}

HRESULT CPlayer::Ready_Weapons()
{
	m_Weapons.resize(WEAPON_END, nullptr);

	CWeapon::WEAPONDESC WeaponDesc;
	WeaponDesc.iTag = (_uint)TAG_PLAYER_WEAPON;
	WeaponDesc.iLevelID = LEVEL_STATIC;
	WeaponDesc.pParentTransform = m_pTransform;
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_l");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Player_Dagger";
	WeaponDesc.pOwner = this;
	WeaponDesc.pColliderDesc = nullptr;

	m_Weapons[DAGGER] = static_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[DAGGER])
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc = {};
	ColliderDesc.eType = CCollider::OBB;
	ColliderDesc.strCollisionLayer = "Player_Weapon";
	ColliderDesc.pOwner = this;
	ColliderDesc.vCenter = { 0.7f, 0.f, 0.f };
	ColliderDesc.vSize = { 1.3f, 0.1f, 0.1f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };
	ColliderDesc.bActive = false;

	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_r");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Player_Saber";
	WeaponDesc.pColliderDesc = &ColliderDesc;
	m_Weapons[SABER] = static_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[SABER])
		return E_FAIL;

	// Claw »ý¼º

	ColliderDesc.eType = CCollider::SPHERE;
	ColliderDesc.vCenter = { 0.2f, 0.f, 0.f };
	ColliderDesc.vSize = { 1.f, 0.f, 0.1f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };
	ColliderDesc.bActive = false;

	WeaponDesc.pSocketBone = m_pModel->Get_Bone("ik_hand_r");
	WeaponDesc.wstrModelTag = L"";
	WeaponDesc.pColliderDesc = &ColliderDesc;
	m_Weapons[CLAW_R] = static_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[CLAW_R])
		return E_FAIL;

	WeaponDesc.pSocketBone = m_pModel->Get_Bone("ik_hand_l");
	m_Weapons[CLAW_L] = static_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[CLAW_L])
		return E_FAIL;

	m_Weapons[CLAW_L]->Set_Active(false);
	m_Weapons[CLAW_R]->Set_Active(false);

	return S_OK;
}

HRESULT CPlayer::Ready_PlagueWeapons()
{
	CCollider::COLLIDERDESC ColliderDesc = {};
	ColliderDesc.eType = CCollider::SPHERE;
	ColliderDesc.strCollisionLayer = "Player_Weapon";
	ColliderDesc.pOwner = this;
	ColliderDesc.vCenter = { 0.7f, 0.f, 0.f };
	ColliderDesc.vSize = { 0.8f, 0.1f, 0.1f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };
	ColliderDesc.bActive = false;

	CWeapon::WEAPONDESC WeaponDesc{};
	WeaponDesc.iTag = (_uint)TAG_PLAYER_WEAPON;
	WeaponDesc.pParentTransform = m_pTransform;
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_l");
	//WeaponDesc.wstrModelTag = L"Prototype_Model_PW_Axe";
	WeaponDesc.pOwner = this;
	WeaponDesc.pColliderDesc = &ColliderDesc;
	
	m_Weapons[PW_AXE] = static_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_PlagueWeapon", &WeaponDesc));
	if (nullptr == m_Weapons[PW_AXE])
		return E_FAIL;

	m_Weapons[PW_AXE]->Set_Active(false);

	ColliderDesc.vCenter = { 1.f, 0.f, 0.f };
	ColliderDesc.vSize = { 1.f, 0.1f, 0.1f };
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_r");
	//WeaponDesc.wstrModelTag = L"Prototype_Model_PW_Hammer";

	m_Weapons[PW_HAMMER] = static_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_PlagueWeapon", &WeaponDesc));
	if (nullptr == m_Weapons[PW_HAMMER])
		return E_FAIL;
	
	m_Weapons[PW_HAMMER]->Set_Active(false);

	ColliderDesc.eType = CCollider::OBB;
	ColliderDesc.vCenter = { 0.5f, 0.f, 0.f };
	ColliderDesc.vSize = { 3.f, 0.5f, 0.5f };
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_r");
	//WeaponDesc.wstrModelTag = L"Prototype_Model_PW_Spear";

	m_Weapons[PW_SPEAR] = static_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_PlagueWeapon", &WeaponDesc));
	if (nullptr == m_Weapons[PW_SPEAR])
		return E_FAIL;

	m_Weapons[PW_SPEAR]->Set_Active(false);

	ColliderDesc.eType = CCollider::SPHERE;
	ColliderDesc.vCenter = { 0.5f, 0.f, 0.f };
	ColliderDesc.vSize = { 1.f, 0.f, 0.f };
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_l");
	//WeaponDesc.wstrModelTag = L"Prototype_Model_PW_TwinBlade";

	m_Weapons[PW_TWINBLADE_L] = static_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_PlagueWeapon", &WeaponDesc));
	if (nullptr == m_Weapons[PW_TWINBLADE_L])
		return E_FAIL;

	m_Weapons[PW_TWINBLADE_L]->Set_Active(false);

	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_r");

	m_Weapons[PW_TWINBLADE_R] = static_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_PlagueWeapon", &WeaponDesc));
	if (nullptr == m_Weapons[PW_TWINBLADE_R])
		return E_FAIL;

	m_Weapons[PW_TWINBLADE_R]->Set_Active(false);

	return S_OK;
}


CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pTargetTransform);

	Safe_Release(m_pStats);
}
