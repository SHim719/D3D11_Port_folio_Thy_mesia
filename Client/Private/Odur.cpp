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

	Change_State((_uint)OdurState::State_CaneAttack1);
	m_pModel->Set_AnimPlay();

	m_pModel->Set_Preview(true);
	return S_OK;
}

void COdur::Tick(_float fTimeDelta)
{
	m_States[m_iState]->OnGoing(fTimeDelta);

	m_pModel->Play_Animation(fTimeDelta);
}

void COdur::LateTick(_float fTimeDelta)
{
	m_pCollider->Update(m_pTransform->Get_WorldMatrix());
	m_pHitBoxCollider->Update(m_pTransform->Get_WorldMatrix());

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
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

	if (FAILED(m_pModel->SetUp_BoneMatrices(m_pShader)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModel->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/


		if (FAILED(m_pModel->Render(m_pShader, i)))
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
	m_pModel->Bind_Func("Active_Odur_Cane_Collider", bind(&CWeapon::Active_Collider, m_Weapons[SWORD]));
	m_pModel->Bind_Func("Inactive_Odur_Cane_Collider", bind(&CWeapon::Active_Collider, m_Weapons[SWORD]));
	m_pModel->Bind_Func("Active_Odur_Foot_Collider", bind(&CWeapon::Active_Collider, m_Weapons[FOOT]));
	m_pModel->Bind_Func("Inactive_Odur_Foot_Collider", bind(&CWeapon::Inactive_Collider, m_Weapons[FOOT]));
}


void COdur::OnCollisionEnter(CGameObject* pOther)
{
	if (TAG_PLAYER_WEAPON == pOther->Get_Tag())
	{
		m_States[m_iState]->OnHit(nullptr);
	}

}

void COdur::OnCollisionExit(CGameObject* pOther)
{

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
	Desc.bActive = true;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("Collider"), (CComponent**)&m_pCollider, &Desc)))
		return E_FAIL;

	Desc.eType = CCollider::SPHERE;
	Desc.pOwner = this;
	Desc.vCenter = { 0.f, 1.3f, 0.f };
	Desc.vSize = { 1.5f, 0.f, 0.0f };
	Desc.vRotation = { 0.f, 0.f, 0.f };
	Desc.strCollisionLayer = "HitBox";
	Desc.bActive = true;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("HitBox"), (CComponent**)&m_pHitBoxCollider, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT COdur::Ready_States()
{
	m_States.resize((_uint)OdurState::State_End);

	m_States[(_uint)OdurState::State_Idle] = COdurState_Idle::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_Hit] = COdurState_Hit::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)OdurState::State_CaneAttack1] = COdurState_CaneAttack1::Create(m_pDevice, m_pContext, this);


	return S_OK;
}

HRESULT COdur::Ready_Weapons()
{
	m_Weapons.resize(WEAPON_END);

	CCollider::COLLIDERDESC ColliderDesc = {};
	ColliderDesc.eType = CCollider::OBB;
	ColliderDesc.pOwner = this;
	ColliderDesc.vCenter = { 0.f, 0.f, 0.2f };
	ColliderDesc.vSize = { 0.1f, 0.1f, 0.9f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };

	CWeapon::WEAPONDESC WeaponDesc;
	WeaponDesc.iTag = (_uint)TAG_ENEMY_WEAPON;
	WeaponDesc.pParentTransform = m_pTransform;
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_Cane");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Odur_Cane";
	WeaponDesc.pColliderDesc = &ColliderDesc;

	m_Weapons[CANE] = static_cast<CWeapon*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Enemy_Weapon", L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[CANE])
		return E_FAIL;

	ColliderDesc.eType = CCollider::OBB;
	ColliderDesc.pOwner = this;
	ColliderDesc.vCenter = { 0.5f, 0.f, 0.f };
	ColliderDesc.vSize = { 1.f, 0.1f, 0.1f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };

	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_r_Sword");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Odur_Sword";
	WeaponDesc.pColliderDesc = &ColliderDesc;
	m_Weapons[SWORD] = static_cast<CWeapon*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Enemy_Weapon", L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[SWORD])
		return E_FAIL;

	WeaponDesc.pSocketBone = m_pModel->Get_Bone("ball_l");
	WeaponDesc.wstrModelTag = L"";
	WeaponDesc.pColliderDesc = &ColliderDesc;

	ColliderDesc.eType = CCollider::SPHERE;
	ColliderDesc.pOwner = this;
	ColliderDesc.vCenter = { 0.f, 0.f, 0.f };
	ColliderDesc.vSize = { 0.8f, 0.f, 0.f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };
	

	m_Weapons[FOOT] = static_cast<CWeapon*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Enemy_Weapon", L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[FOOT])
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
