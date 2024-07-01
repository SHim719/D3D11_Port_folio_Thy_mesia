#include "Player.h"

#include "Player_States.h"
#include "PlayerStats.h"

#include "Weapon.h"

#include "Enemy.h"

#include "Cutscene_Manager.h"
#include "UI_Manager.h"
#include "UI_DamageFont.h"

#include "Bone.h"

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

	Bind_KeyFrames();

	CEnemy::Set_Target(this);

	m_pTransform->Set_MoveLook(m_pTransform->Get_Look());

	m_pModel->Set_AnimPlay();

	Change_State((_uint)PlayerState::State_Idle);
	m_pModel->Change_Animation(Corvus_SD_Idle, 0.f);

	m_pTransform->Set_Position(XMVectorSet(37.326f, 0.798f, -2.175f, 1.f));

	CUTSCENEMGR->Add_Actor(this);


	return S_OK;
}

void CPlayer::PriorityTick(_float fTimeDelta)
{
	__super::PriorityTick(fTimeDelta);

	Update_CanExecutionEnemy();
}

void CPlayer::Tick(_float fTimeDelta)
{
	if (m_bLockOn)
		m_pTransform->LookAt2D(m_pTargetTransform->Get_Position());

	if (m_pGameInstance->GetKeyNone(eKeyCode::One))
		m_States[m_iState]->Update(fTimeDelta);

	m_pModel->Play_Animation(fTimeDelta);
}

void CPlayer::LateTick(_float fTimeDelta)
{
	if (m_pGameInstance->GetKeyNone(eKeyCode::One))
		m_States[m_iState]->Late_Update(fTimeDelta);

	if (m_bAdjustNaviY)
		Compute_YPos();

	m_pCollider->Update(m_pTransform->Get_WorldMatrix());
	m_pHitBoxCollider->Update(m_pTransform->Get_WorldMatrix());

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayer::Render()
{
	if (m_bNoRender)
		return S_OK;

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	m_pModel->SetUp_BoneMatrices(m_pShader);

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint j = 0; j < iNumMeshes; ++j)
	{
		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, j, TextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;


		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModel->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/


		if (FAILED(m_pModel->Render(m_pShader, j, 0)))
			return E_FAIL;
	}

	m_pCollider->Render();
	m_pHitBoxCollider->Render();
	m_pNavigation->Render();

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
}

void CPlayer::OnEnd_Cutscene()
{
	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&m_PrevWorldMatrix));

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
}

void CPlayer::Update_CanExecutionEnemy()
{
	m_pExecutionTarget = nullptr;

	if (m_bLockOn)
	{
		_float fAngle = JoMath::Calc_AngleToTarget(m_pTransform->Get_Position(), m_pTargetTransform->Get_Position(), m_pTransform->Get_GroundLook());
		if (fabsf(fAngle) < To_Radian(70.f))
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
				if (fabsf(fAngle) < To_Radian(70.f))
				{
					_float fDist = XMVector3Length(m_pTransform->Get_Position() - (*it)->Get_Transform()->Get_Position()).m128_f32[0];
					if (fDist < m_fExecutionDist && fDist < fMaxDist)
					{
						fMaxDist = fDist;
						m_pExecutionTarget = *it;
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

void CPlayer::OnCollisionEnter(CGameObject* pOther)
{
	if (TAG_ENEMY_WEAPON == pOther->Get_Tag())
	{
		CWeapon* pEnemyWeapon = static_cast<CWeapon*>(pOther);

		m_States[m_iState]->OnHit(pEnemyWeapon->Get_AttackDesc());
	}
}

_int CPlayer::Take_Damage(const ATTACKDESC& AttackDesc)
{
	CUI_DamageFont::DAMAGEFONTDESC DamageFontDesc;
	DamageFontDesc.strDamage = to_string(AttackDesc.iDamage);
	XMStoreFloat4(&DamageFontDesc.vWorldPosition, JoMath::Get_BoneWorldPos(m_pModel->Get_Bone("Bip001-Head"), m_pTransform->Get_WorldMatrix()));

	UIMGR->Active_UI("UI_DamageFont", &DamageFontDesc);

	return m_pStats->Increase_Hp(-AttackDesc.iDamage);
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

	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelID(), TEXT("Prototype_Model_Player"), TEXT("Model"), (CComponent**)&m_pModel)))
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

	CNavigation::NAVIGATION_DESC NaviDesc;
	NaviDesc.iCurrentCellIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Navigation"), TEXT("Navigation"), (CComponent**)&m_pNavigation, &NaviDesc)))
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
	m_States[(_uint)PlayerState::State_StealRush] = CPlayerState_StealRush::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Parried] = CPlayerState_Parried::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Execution_Default] = CPlayerState_Execution_Default::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Execution_Joker] = CPlayerState_Execution_Joker::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Executed] = CPlayerState_Executed::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Finish] = CPlayerState_Finish::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Cutscene] = CPlayerState_Cutscene::Create(m_pDevice, m_pContext, this);

	return S_OK;
}

HRESULT CPlayer::Ready_Weapons()
{
	m_Weapons.resize(WEAPON_END, nullptr);

	CWeapon::WEAPONDESC WeaponDesc;
	WeaponDesc.iTag = (_uint)TAG_PLAYER_WEAPON;
	WeaponDesc.pParentTransform = m_pTransform;
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_l");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Player_Dagger";
	WeaponDesc.pOwner = this;
	WeaponDesc.pColliderDesc = nullptr;

	m_Weapons[DAGGER] = static_cast<CWeapon*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Player_Weapon", L"Prototype_Weapon", &WeaponDesc));
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
	m_Weapons[SABER] = static_cast<CWeapon*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Player_Weapon", L"Prototype_Weapon", &WeaponDesc));
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
	m_Weapons[CLAW_R] = static_cast<CWeapon*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Player_Weapon", L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[CLAW_R])
		return E_FAIL;

	WeaponDesc.pSocketBone = m_pModel->Get_Bone("ik_hand_l");
	m_Weapons[CLAW_L] = static_cast<CWeapon*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Player_Weapon", L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[CLAW_L])
		return E_FAIL;

	m_Weapons[CLAW_L]->Set_Active(false);
	m_Weapons[CLAW_R]->Set_Active(false);

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
