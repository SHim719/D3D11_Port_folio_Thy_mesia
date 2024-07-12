#include "HalberdKnight.h"
#include "HalberdKnight_States.h"
#include "Weapon.h"
#include "Bone.h"
#include "PerceptionBounding.h"

#include "UI_Manager.h"
#include "UI_EnemyBar.h"

CHalberdKnight::CHalberdKnight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CHalberdKnight::CHalberdKnight(const CHalberdKnight& rhs)
	: CEnemy(rhs)
{
}

HRESULT CHalberdKnight::Initialize_Prototype()
{
	m_iTag = (_uint)TAG_ENEMY;
	return S_OK;
}

HRESULT CHalberdKnight::Initialize(void* pArg)
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

	Change_State((_uint)HalberdKnight_State::State_Idle);
	m_pModel->Set_AnimPlay();

	return S_OK;
}

void CHalberdKnight::Tick(_float fTimeDelta)
{
	if (m_bLookTarget)
	{
		m_pTransform->Rotation_Quaternion(
			JoMath::Slerp_TargetLook(m_pTransform->Get_GroundLook()
				, JoMath::Calc_GroundLook(s_pTarget->Get_Transform()->Get_Position(), m_pTransform->Get_Position())
				, m_fRotRate * fTimeDelta));
	}

	//m_States[m_iState]->Update(fTimeDelta);

	if (m_bAdjustNaviY)
		m_pNavigation->Decide_YPos(m_pTransform->Get_Position());

	__super::Update_Colliders();

	__super::Tick_Weapons(fTimeDelta);

	m_pModel->Play_Animation(fTimeDelta);
}

void CHalberdKnight::LateTick(_float fTimeDelta)
{
	//m_States[m_iState]->Late_Update(fTimeDelta);

	__super::LateTick_Weapons(fTimeDelta);

	if (m_bNoRender)
		return;

#ifdef _DEBUG
	m_pGameInstance->Add_RenderComponent(m_pCollider);
	m_pGameInstance->Add_RenderComponent(m_pHitBoxCollider);
#endif

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CHalberdKnight::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
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

		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, 0)))
			return E_FAIL;
	}

	return S_OK;
}


void CHalberdKnight::Bind_KeyFrames()
{
	//m_pModel->Bind_Func("Active_WeaponCollider", bind(&CWeapon::Set_Active_Collider, m_Weapons[HAND], true));
	//m_pModel->Bind_Func("Inactive_WeaponCollider", bind(&CWeapon::Set_Active_Collider, m_Weapons[HAND], false));
	//m_pModel->Bind_Func("Enable_LookTarget", bind(&CEnemy::Set_LookTarget, this, true));
	//m_pModel->Bind_Func("Disable_LookTarget", bind(&CEnemy::Set_LookTarget, this, false));
	//m_pModel->Bind_Func("Enable_Stanced", bind(&CCharacter::Set_Stanced, this, true));
	//m_pModel->Bind_Func("Disable_Stanced", bind(&CCharacter::Set_Stanced, this, false));
	//m_pModel->Bind_Func("Update_AttackDesc", bind(&CCharacter::Update_AttackDesc, this));
}

void CHalberdKnight::Percept_Target()
{
	_int iRandNum = JoRandom::Random_Int(0, 1);

	if (iRandNum)
		Change_State((_uint)HalberdKnight_State::State_Walk);
	else
		Change_State((_uint)HalberdKnight_State::State_Attack3);
}

void CHalberdKnight::SetState_Death()
{
	Change_State((_uint)HalberdKnight_State::State_Death);
}

HRESULT CHalberdKnight::Ready_Components(void* pArg)
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

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Model_HalberdKnight"), TEXT("Model"), (CComponent**)&m_pModel)))
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

HRESULT CHalberdKnight::Ready_States()
{
	m_States.resize((_uint)HalberdKnight_State::State_End);

	//m_States[(_uint)HalberdKnight_State::State_Idle] = CHalberdKnightState_Idle::Create(m_pDevice, m_pContext, this);
	//m_States[(_uint)HalberdKnight_State::State_Walk] = CHalberdKnightState_Walk::Create(m_pDevice, m_pContext, this);
	//m_States[(_uint)HalberdKnight_State::State_Attack1] = CHalberdKnightState_Attack1::Create(m_pDevice, m_pContext, this);
	//m_States[(_uint)HalberdKnight_State::State_Attack2] = CHalberdKnightState_Attack2::Create(m_pDevice, m_pContext, this);
	//m_States[(_uint)HalberdKnight_State::State_Attack3] = CHalberdKnightState_Attack3::Create(m_pDevice, m_pContext, this);
	//m_States[(_uint)HalberdKnight_State::State_Stunned_Start] = CHalberdKnightState_Stunned_Start::Create(m_pDevice, m_pContext, this);
	//m_States[(_uint)HalberdKnight_State::State_Stunned_Loop] = CHalberdKnightState_Stunned_Loop::Create(m_pDevice, m_pContext, this);
	//m_States[(_uint)HalberdKnight_State::State_Death] = CHalberdKnightState_Death::Create(m_pDevice, m_pContext, this);
	//m_States[(_uint)HalberdKnight_State::State_Hit] = CHalberdKnightState_Hit::Create(m_pDevice, m_pContext, this);


	return S_OK;
}

HRESULT CHalberdKnight::Ready_Weapon()
{
	m_Weapons.resize(WEAPON_END);

	CCollider::COLLIDERDESC ColliderDesc = {};
	ColliderDesc.eType = CCollider::OBB;
	ColliderDesc.pOwner = this;
	ColliderDesc.vCenter = { 0.6f, 0.f, 0.f };
	ColliderDesc.vSize = { 2.f, 0.1f, 0.1f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };
	ColliderDesc.bActive = false;
	ColliderDesc.strCollisionLayer = "Enemy_Weapon";

	CWeapon::WEAPONDESC WeaponDesc;
	WeaponDesc.iTag = (_uint)TAG_ENEMY_WEAPON;
	WeaponDesc.pParentTransform = m_pTransform;
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_r_pivot");
	WeaponDesc.pOwner = this;
	WeaponDesc.pColliderDesc = &ColliderDesc;

	//m_Weapons[HAND] = static_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_Weapon", &WeaponDesc));
	//if (nullptr == m_Weapons[HAND])
	//	return E_FAIL;

	return S_OK;
}

HRESULT CHalberdKnight::Ready_Stats()
{
	ENEMYDESC EnemyDesc;
	EnemyDesc.wstrEnemyName = L"¸ó½ºÅÍ1";
	EnemyDesc.iMaxHp = 100;

	m_pStats = CEnemyStats::Create(EnemyDesc);

	return S_OK;
}

HRESULT CHalberdKnight::Ready_UI()
{
	CUI_EnemyBar::UIENEMYBARDESC EnemyBarDesc;
	EnemyBarDesc.pStats = m_pStats;
	EnemyBarDesc.vOffset.y = 2.5f;
	EnemyBarDesc.pOwnerTransform = m_pTransform;

	UIMGR->Active_UI("UI_EnemyBar", &EnemyBarDesc);

	return S_OK;
}


CHalberdKnight* CHalberdKnight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHalberdKnight* pInstance = new CHalberdKnight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHalberdKnight"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHalberdKnight::Clone(void* pArg)
{
	CHalberdKnight* pInstance = new CHalberdKnight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CHalberdKnight"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHalberdKnight::Free()
{
	__super::Free();
}
