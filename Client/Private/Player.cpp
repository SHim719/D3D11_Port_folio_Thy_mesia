#include "Player.h"

#include "Player_States.h"
#include "PlayerStat.h"

#include "Weapon.h"

#include "Enemy.h"

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

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	if (m_bLockOn)
		m_pTransform->LookAt2D(m_pTargetTransform->Get_Position());

	if (m_pGameInstance->GetKeyNone(eKeyCode::RButton))
		m_States[m_iState]->OnGoing(fTimeDelta);

	m_pModel->Play_Animation(fTimeDelta);
}

void CPlayer::LateTick(_float fTimeDelta)
{
	m_pCollider->Update(m_pTransform->Get_WorldMatrix());
	m_pHitBoxCollider->Update(m_pTransform->Get_WorldMatrix());

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayer::Render()
{
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

	return S_OK;
}

void CPlayer::Bind_KeyFrames()
{
	m_pModel->Bind_Func("Active_SaberCollider", bind(&CWeapon::Active_Collider, m_Weapons[SABER]));
	m_pModel->Bind_Func("Inactive_SaberCollider", bind(&CWeapon::Inactive_Collider, m_Weapons[SABER]));
	m_pModel->Bind_Func("Enable_NextState", bind(&CPlayer::Enable_NextState, this));
	m_pModel->Bind_Func("Disable_NextState", bind(&CPlayer::Disable_NextState, this));
	m_pModel->Bind_Func("Disable_Rotation", bind(&CPlayer::Disable_Rotation, this));
	m_pModel->Bind_Func("Set_Vulnerable", bind(&CPlayer::Set_Vulnerable, this));

}



void CPlayer::Active_Weapons()
{
	m_Weapons[DAGGER]->Set_Active(true);
	m_Weapons[SABER]->Set_Active(true);
}

void CPlayer::InActive_Weapons()
{
	m_Weapons[DAGGER]->Set_Active(false);
	m_Weapons[SABER]->Set_Active(false);
}

void CPlayer::Active_Claw()
{
}

void CPlayer::InActive_Claw()
{
}

void CPlayer::Toggle_LockOn(CTransform* pTargetTransform)
{
	m_bLockOn = !m_bLockOn;

	Safe_Release(m_pTargetTransform);

	m_pTargetTransform = pTargetTransform;

	Safe_AddRef(m_pTargetTransform);
}

void CPlayer::OnCollisionEnter(CGameObject* pOther)
{
	if (TAG_ENEMY_WEAPON == pOther->Get_Tag())
	{
		m_States[m_iState]->OnHit(nullptr);
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

	return S_OK;
}

HRESULT CPlayer::Ready_States()
{
	m_pStats = CPlayerStat::Create();

	m_States.resize((_uint)PlayerState::State_End);

	m_States[(_uint)PlayerState::State_Idle] = CPlayerState_Idle::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Jog] = CPlayerState_Jog::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Sprint] = CPlayerState_Sprint::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Avoid] = CPlayerState_Avoid::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Attack] = CPlayerState_Attack::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_LockOn] = CPlayerState_LockOn::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Parry] = CPlayerState_Parry::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_ParrySuccess] = CPlayerState_ParrySuccess::Create(m_pDevice, m_pContext, this);

	return S_OK;
}

HRESULT CPlayer::Ready_Weapons()
{
	m_Weapons.resize(WEAPON_END);

	CWeapon::WEAPONDESC WeaponDesc;
	WeaponDesc.iTag = (_uint)TAG_PLAYER_WEAPON;
	WeaponDesc.pParentTransform = m_pTransform;
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_l");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Player_Dagger";
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
