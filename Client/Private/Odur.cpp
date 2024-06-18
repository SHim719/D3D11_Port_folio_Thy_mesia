#include "Odur.h"
#include "Odur_States.h"
#include "Weapon.h"
#include "Bone.h"

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
	return S_OK;
}

HRESULT COdur::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	if (FAILED(Ready_Weapons()))
		return E_FAIL;

	Bind_KeyFrames();

	m_pSwapBone = m_pModel->Get_Bone("weapon_l_Sword");
	Safe_AddRef(m_pSwapBone);

	Change_State((_uint)OdurState::State_Idle);
	m_pModel->Set_AnimPlay();
	return S_OK;
}

void COdur::Tick(_float fTimeDelta)
{
	if (KEY_DOWN(eKeyCode::T))
		Change_State((_uint)OdurState::State_ReadyExecution);

	if (m_bLookTarget)
	{
		m_pTransform->Rotation_Quaternion(
			JoMath::Slerp_TargetLook(m_pTransform->Get_GroundLook()
				, JoMath::Calc_GroundLook(s_pTarget->Get_Transform()->Get_Position(), m_pTransform->Get_Position())
				, m_fRotRate * fTimeDelta));
	}

	m_States[m_iState]->OnGoing(fTimeDelta);

	m_pModel->Play_Animation(fTimeDelta);
}

void COdur::LateTick(_float fTimeDelta)
{
	Update_Alpha(fTimeDelta);

	m_pCollider->Update(m_pTransform->Get_WorldMatrix());
	m_pHitBoxCollider->Update(m_pTransform->Get_WorldMatrix());

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
}

HRESULT COdur::Render()
{
	if (nullptr == m_pModel ||
		nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
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

		if (FAILED(m_pModel->Render(m_pShader, i, 1)))
			return E_FAIL;  
	}

	m_pCollider->Render();
	m_pHitBoxCollider->Render();

	return S_OK;
}

void COdur::Bind_KeyFrames()
{
	m_pModel->Bind_Func("Active_Odur_Cane_Collider", bind(&CWeapon::Active_Collider, m_Weapons[CANE]));
	m_pModel->Bind_Func("Inactive_Odur_Cane_Collider", bind(&CWeapon::Inactive_Collider, m_Weapons[CANE]));
	m_pModel->Bind_Func("Active_Odur_Sword_Collider", bind(&CWeapon::Active_Collider, m_Weapons[SWORD]));
	m_pModel->Bind_Func("Inactive_Odur_Sword_Collider", bind(&CWeapon::Inactive_Collider, m_Weapons[SWORD]));
	m_pModel->Bind_Func("Active_Odur_Foot_L_Collider", bind(&CWeapon::Active_Collider, m_Weapons[FOOT_L]));
	m_pModel->Bind_Func("Inactive_Odur_Foot_L_Collider", bind(&CWeapon::Inactive_Collider, m_Weapons[FOOT_L]));
	m_pModel->Bind_Func("Active_Odur_Foot_R_Collider", bind(&CWeapon::Active_Collider, m_Weapons[FOOT_R]));
	m_pModel->Bind_Func("Inactive_Odur_Foot_R_Collider", bind(&CWeapon::Inactive_Collider, m_Weapons[FOOT_R]));
	m_pModel->Bind_Func("Swap_Bone", bind(&COdur::Swap_Bone, this));
	m_pModel->Bind_Func("Enable_LookTarget", bind(&CEnemy::Enable_LookTarget, this));
	m_pModel->Bind_Func("Disable_LookTarget", bind(&CEnemy::Disable_LookTarget, this));
	m_pModel->Bind_Func("Enable_Stanced", bind(&CCharacter::Enable_Stanced, this));
	m_pModel->Bind_Func("Disable_Stanced", bind(&CCharacter::Disable_Stanced, this));
	m_pModel->Bind_Func("Add_AttackIdx", bind(&CCharacter::Add_AttackIdx, this));
}

void COdur::Swap_Bone()
{
	m_Weapons[SWORD]->Swap_SocketBone(m_pSwapBone);
}

void COdur::Update_Alpha(_float fTimeDelta)
{
	if (false == m_bAlphaEnable)
		return;
		
	_float fDeltaSpeed = m_fDeltaAlphaSpeed * (m_bAlphaIncrease == false ? -1.f : 1.f);

	m_fAlpha += fDeltaSpeed * fTimeDelta;

	m_fAlpha = clamp(m_fAlpha, 0.f, 1.f);

	if (1.f == m_fAlpha || 0.f == m_fAlpha)
		m_bAlphaEnable = false;

	Update_WeaponAlpha();
}

void COdur::Update_WeaponAlpha()
{
	for (auto pWeapon : m_Weapons)
		pWeapon->Set_Alpha(m_fAlpha);
}


void COdur::OnCollisionEnter(CGameObject* pOther)
{
	__super::OnCollisionEnter(pOther);

	if (TAG_PLAYER_WEAPON == pOther->Get_Tag())
	{
		if (false == m_bStanced)
			m_States[m_iState]->OnHit(nullptr);


	}

}

void COdur::OnCollisionExit(CGameObject* pOther)
{
	__super::OnCollisionExit(pOther);
}

HRESULT COdur::Ready_Components()
{
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
	m_States[(_uint)OdurState::State_ReadyExecution] = COdurState_ReadyExecution::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_Execute] = COdurState_Execute::Create(m_pDevice, m_pContext, this);

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
	WeaponDesc.pParentTransform = m_pTransform;
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_Cane");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Odur_Cane";
	WeaponDesc.pOwner = this;
	WeaponDesc.pColliderDesc = &ColliderDesc;
	WeaponDesc.bAlphaBlend = true;

	m_Weapons[CANE] = static_cast<CWeapon*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Enemy_Weapon", L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[CANE])
		return E_FAIL;

	ColliderDesc.eType = CCollider::OBB;
	ColliderDesc.vCenter = { 0.7f, 0.f, 0.f };
	ColliderDesc.vSize = { 1.3f, 0.1f, 0.1f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };

	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_r_Sword");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Odur_Sword";
	m_Weapons[SWORD] = static_cast<CWeapon*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Enemy_Weapon", L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[SWORD])
		return E_FAIL;

	WeaponDesc.pSocketBone = m_pModel->Get_Bone("ball_l");
	WeaponDesc.wstrModelTag = L"";
	WeaponDesc.pColliderDesc = &ColliderDesc;

	ColliderDesc.eType = CCollider::SPHERE;
	ColliderDesc.vCenter = { 0.f, 0.f, 0.f };
	ColliderDesc.vSize = { 1.f, 0.f, 0.f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };

	m_Weapons[FOOT_L] = static_cast<CWeapon*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Enemy_Weapon", L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[FOOT_L])
		return E_FAIL;

	WeaponDesc.pSocketBone = m_pModel->Get_Bone("ball_r");
	m_Weapons[FOOT_R] = static_cast<CWeapon*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Enemy_Weapon", L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[FOOT_R])
		return E_FAIL;



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
