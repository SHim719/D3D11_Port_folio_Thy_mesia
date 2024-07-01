#include "Villager_F.h"
#include "VillagerF_States.h"
#include "Weapon.h"
#include "Bone.h"
#include "PerceptionBounding.h"

#include "UI_Manager.h"
#include "UI_EnemyBar.h"

CVillager_F::CVillager_F(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CVillager_F::CVillager_F(const CVillager_F& rhs)
	: CEnemy(rhs)
{
}

HRESULT CVillager_F::Initialize_Prototype()
{
	m_iTag = (_uint)TAG_ENEMY;
	return S_OK;
}

HRESULT CVillager_F::Initialize(void* pArg)
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

	Change_State((_uint)VillagerF_State::State_Idle);
	m_pModel->Set_AnimPlay();

	return S_OK;
}

void CVillager_F::Tick(_float fTimeDelta)
{
	if (m_bLookTarget)
	{
		m_pTransform->Rotation_Quaternion(
			JoMath::Slerp_TargetLook(m_pTransform->Get_GroundLook()
				, JoMath::Calc_GroundLook(s_pTarget->Get_Transform()->Get_Position(), m_pTransform->Get_Position())
				, m_fRotRate * fTimeDelta));
	}

	m_States[m_iState]->Update(fTimeDelta);

	m_pModel->Play_Animation(fTimeDelta);
}

void CVillager_F::LateTick(_float fTimeDelta)
{
	m_States[m_iState]->Late_Update(fTimeDelta);

	m_pNavigation->Decide_YPos(m_pTransform->Get_Position());

	m_pCollider->Update(m_pTransform->Get_WorldMatrix());
	m_pHitBoxCollider->Update(m_pTransform->Get_WorldMatrix());

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CVillager_F::Render()
{
	if (m_bNoRender)
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

		if (FAILED(m_pModel->Render(m_pShader, i, 0)))
			return E_FAIL;
	}

	m_pCollider->Render();
	m_pHitBoxCollider->Render();

	return S_OK;
}


void CVillager_F::Bind_KeyFrames()
{
	m_pModel->Bind_Func("Active_WeaponCollider", bind(&CWeapon::Set_Active_Collider, m_Weapons[HAND], true));
	m_pModel->Bind_Func("Inactive_WeaponCollider", bind(&CWeapon::Set_Active_Collider, m_Weapons[HAND], false));
	m_pModel->Bind_Func("Enable_LookTarget", bind(&CEnemy::Set_LookTarget, this, true));
	m_pModel->Bind_Func("Disable_LookTarget", bind(&CEnemy::Set_LookTarget, this, false));
	m_pModel->Bind_Func("Enable_Stanced", bind(&CCharacter::Set_Stanced, this, true));
	m_pModel->Bind_Func("Disable_Stanced", bind(&CCharacter::Set_Stanced, this, false));
	m_pModel->Bind_Func("Update_AttackDesc", bind(&CCharacter::Update_AttackDesc, this));
}

void CVillager_F::OnCollisionEnter(CGameObject* pOther)
{
	__super::OnCollisionEnter(pOther);

	if (TAG_PLAYER_WEAPON == pOther->Get_Tag())
	{
		m_States[m_iState]->OnHit(static_cast<CWeapon*>(pOther)->Get_AttackDesc());
	}

}

void CVillager_F::Percept_Target()
{
	_int iRandNum = JoRandom::Random_Int(0, 1);

	if (iRandNum)
		Change_State((_uint)VillagerF_State::State_Walk);
	else
		Change_State((_uint)VillagerF_State::State_Attack3);
}

void CVillager_F::SetState_Death()
{
	Change_State((_uint)VillagerF_State::State_Death);
}

HRESULT CVillager_F::Ready_Components(void* pArg)
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

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Model_Villager_F"), TEXT("Model"), (CComponent**)&m_pModel)))
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

	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Navigation"), TEXT("Navigation"), (CComponent**)&m_pNavigation, &(pLoadDesc->iNaviIdx))))
		return E_FAIL;

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pLoadDesc->WorldMatrix));

	return S_OK;
}

HRESULT CVillager_F::Ready_States()
{
	m_States.resize((_uint)VillagerF_State::State_End);

    m_States[(_uint)VillagerF_State::State_Idle] = CVillagerFState_Idle::Create(m_pDevice, m_pContext, this);
    m_States[(_uint)VillagerF_State::State_Walk] = CVillagerFState_Walk::Create(m_pDevice, m_pContext, this);
    m_States[(_uint)VillagerF_State::State_Attack1] = CVillagerFState_Attack1::Create(m_pDevice, m_pContext, this);
    m_States[(_uint)VillagerF_State::State_Attack2] = CVillagerFState_Attack2::Create(m_pDevice, m_pContext, this);
    m_States[(_uint)VillagerF_State::State_Attack3] = CVillagerFState_Attack3::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VillagerF_State::State_Stunned_Start] = CVillagerFState_Stunned_Start::Create(m_pDevice, m_pContext, this);
    m_States[(_uint)VillagerF_State::State_Stunned_Loop] = CVillagerFState_Stunned_Loop::Create(m_pDevice, m_pContext, this);
    m_States[(_uint)VillagerF_State::State_Death] = CVillagerFState_Death::Create(m_pDevice, m_pContext, this);
    m_States[(_uint)VillagerF_State::State_Hit] = CVillagerFState_Hit::Create(m_pDevice, m_pContext, this);


	return S_OK;
}

HRESULT CVillager_F::Ready_Weapon()
{
	m_Weapons.resize(WEAPON_END);
	
	CCollider::COLLIDERDESC ColliderDesc = {};
	ColliderDesc.eType = CCollider::SPHERE;
	ColliderDesc.pOwner = this;
	ColliderDesc.vCenter = { 0.f, 0.f, 0.f };
	ColliderDesc.vSize = { 0.3f, 0.1f, 0.1f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };
	ColliderDesc.bActive = false;
	ColliderDesc.strCollisionLayer = "Enemy_Weapon";
	
	CWeapon::WEAPONDESC WeaponDesc;
	WeaponDesc.iTag = (_uint)TAG_ENEMY_WEAPON;
	WeaponDesc.pParentTransform = m_pTransform;
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("hand_r");
	WeaponDesc.pOwner = this;
	WeaponDesc.pColliderDesc = &ColliderDesc;
	
	m_Weapons[HAND] = static_cast<CWeapon*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Enemy_Weapon", L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_Weapons[HAND])
		return E_FAIL;

	return S_OK;
}

HRESULT CVillager_F::Ready_Stats()
{
	ENEMYDESC EnemyDesc;
	EnemyDesc.wstrEnemyName = L"¸ó½ºÅÍ1";
	EnemyDesc.iMaxHp = 100;

	m_pStats = CEnemyStats::Create(EnemyDesc);

	return S_OK;
}

HRESULT CVillager_F::Ready_UI()
{
	CUI_EnemyBar::UIENEMYBARDESC EnemyBarDesc;
	EnemyBarDesc.pStats = m_pStats;
	EnemyBarDesc.vOffset.y = 2.5f;
	EnemyBarDesc.pOwnerTransform = m_pTransform;

	UIMGR->Active_UI("UI_EnemyBar", &EnemyBarDesc);

	return S_OK;
}


CVillager_F* CVillager_F::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVillager_F* pInstance = new CVillager_F(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CVillager_F"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVillager_F::Clone(void* pArg)
{
	CVillager_F* pInstance = new CVillager_F(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CVillager_F"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillager_F::Free()
{
	__super::Free();
}
