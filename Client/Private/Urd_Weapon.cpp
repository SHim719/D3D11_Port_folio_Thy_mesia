#include "Urd_Weapon.h"

#include "Bone.h"

#include "Effect_Trail.h"

#include "Urd.h"
#include "Player.h"
#include "Player_Enums.h"

#include "Effect_Manager.h"
#include "Effect_Hit.h"

#include "Urd_MagicCircle.h"


CUrd_Weapon::CUrd_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon(pDevice, pContext)
{
}

CUrd_Weapon::CUrd_Weapon(const CUrd_Weapon& rhs)
	: CWeapon(rhs)
{
}

HRESULT CUrd_Weapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUrd_Weapon::Initialize(void* pArg)
{
	WEAPONDESC* pWeaponDesc = (WEAPONDESC*)pArg;

	if (FAILED(Ready_Components(pWeaponDesc)))
		return E_FAIL;

	m_pTransform->Set_Speed(30.f);

	m_iTag = pWeaponDesc->iTag;
	m_bAlphaEnabled = pWeaponDesc->bAlphaBlend;

	m_pOwner = pWeaponDesc->pOwner;
	m_pSocketBone = pWeaponDesc->pSocketBone;
	m_pParentTransform = pWeaponDesc->pParentTransform;

	Safe_AddRef(m_pSocketBone);
	Safe_AddRef(m_pParentTransform);

	Init_AttackDesc();

	if (pWeaponDesc->bTrail)
		m_pEffect_Trail = static_cast<CEffect_Trail*>(m_pGameInstance->Clone_GameObject(pWeaponDesc->wstrTrailTag));

	m_pMagicCircle = static_cast<CUrd_MagicCircle*>(m_pGameInstance->Clone_GameObject(L"Prototype_Urd_MagicCircle"));

	m_pEffect_SwordDefault = EFFECTMGR->Get_EffectGroup("Effect_Urd_Sword_Effect");
	m_pEffect_SwordDefault->Set_Using(true);
	Safe_AddRef(m_pEffect_SwordDefault);

	m_EffectSpawnDesc.pParentTransform = m_pTransform;
	m_EffectSpawnDesc.pParentModel = m_pModel;

	m_pModel->Set_AnimPlay();
	m_pModel->Change_Animation(0);
	m_pModel->Set_RootBoneIdx(0);

	return S_OK;
}

void CUrd_Weapon::Tick(_float fTimeDelta)
{
	if (false == m_bReleased)
	{
		m_pTransform->Attach_To_Bone(m_pSocketBone, m_pParentTransform, XMMatrixTranslation(0.5f, 0.f, 0.f));
	}
	else if (m_bThrow)
	{
		if (false == m_bLanded)
		{
			if (false == m_bParried) // 패링 치지 않았을때는 플레이어를 향해 돌진.
				m_pTransform->Go_Dir(m_pTransform->Get_MoveLook(), fTimeDelta);
			else // 패링쳐졌을때 루트애니메이션 재생 
			{
				// 루트애니메이션이 끝나도 땅에 닿지않을 수 있으므로 루트애니메이션이 끝나면 -y방향으로 땅에 닿을 때까지 이동.
				if (false == m_pModel->Is_AnimComplete())
					m_pTransform->Move_Root(m_pModel->Get_DeltaRootPos());
				else
					m_pTransform->Go_Dir(-YAXIS, fTimeDelta);
				
			}
				
			
			if (XMVectorGetY(m_pTransform->Get_Position()) < 0.3f)
			{
				_vector vPos = m_pTransform->Get_Position();
				vPos.m128_f32[1] = 0.3f;
				m_pTransform->Set_Position(vPos);
				m_bLanded = true;
				m_pCollider->Set_Active(false);
				if (false == m_bParried)
					Active_LandingImpact();
				
				static_cast<CUrd*>(m_pOwner)->Active_MagicCircle(false);
			}
		}
	}

	if (m_pModel)
		m_pModel->Play_Animation(fTimeDelta);

	if (m_bTrailActivated)
	{
		m_pEffect_Trail->Update_ParentMatrix(m_pTransform->Get_WorldMatrix());
		m_pEffect_Trail->Tick(fTimeDelta);
	}

	if (m_pCollider)
		m_pCollider->Update(m_pTransform->Get_WorldMatrix());
}


HRESULT CUrd_Weapon::Render()
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

		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;

		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, 0)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CUrd_Weapon::OnEnter_Layer(void* pArg)
{
	__super::OnEnter_Layer(pArg);

	m_pEffect_SwordDefault->OnEnter_Layer(&m_EffectSpawnDesc);
	Safe_AddRef(m_pEffect_SwordDefault);

	m_pGameInstance->Insert_GameObject(GET_CURLEVEL, L"Effect", m_pEffect_SwordDefault);

	return S_OK;
}

void CUrd_Weapon::Released(_bool bThrow)
{
	m_bThrow = bThrow;
	m_bReleased = true;
	m_bUsing = true;
	if (m_bThrow)
	{
		m_bLanded = false;
		m_pTransform->Attach_To_Bone(m_pSocketBone, m_pParentTransform, XMMatrixTranslation(0.5f, 0.f, 0.f));

		_vector vTargetDir = XMVector3Normalize(GET_PLAYER->Get_Transform()->Get_Position() - m_pTransform->Get_Position());
		m_pTransform->Set_MoveLook(vTargetDir);
		m_pCollider->Set_Active(true);
	}
	else
	{
		Active_LandingImpact();
	}
	
}

void CUrd_Weapon::Active_MagicCircle(_bool bUltimate)
{
	if (false == m_bUsing || false == m_bLanded)
		return;
	
	Safe_AddRef(m_pMagicCircle);
	CUrd_MagicCircle::URD_MAGICCIRCLE_DESC Desc;
	XMStoreFloat4(&Desc.vPosition, m_pTransform->Get_Position());
	Desc.bUltimate = bUltimate;
	
	m_pMagicCircle->OnEnter_Layer(&Desc);
	m_pGameInstance->Insert_GameObject(GET_CURLEVEL, L"Effect", m_pMagicCircle);
}

void CUrd_Weapon::Explode_MagicCircle()
{
	m_pMagicCircle->Explosion();
}

void CUrd_Weapon::Active_LandingImpact()
{
	CEffect_Hit::EFFECT_HITDESC EffectHitDesc{};
	CCollider::COLLIDERDESC ColliderDesc{};
	ColliderDesc.eType = CCollider::SPHERE;
	ColliderDesc.pOwner = this;
	ColliderDesc.strCollisionLayer = "Enemy_Weapon";
	ColliderDesc.vSize = { 3.f, 3.f, 1.f };
	ColliderDesc.bActive = true;

	EffectHitDesc.pColliderDesc = &ColliderDesc;
	EffectHitDesc.AttackDesc = m_LandingImpactAtkDesc;
	EffectHitDesc.iTag = TAG_ENEMY_WEAPON;
	XMStoreFloat4(&EffectHitDesc.vSpawnPos, m_pTransform->Get_Position());

	EFFECTMGR->Create_Effect_Hit("Effect_Urd_Sword_Impact", &EffectHitDesc);

	//static_cast<CMain_Camera*>(GET_CAMERA)->Play_CameraShake("Shaking_Joker_Impact");
}

void CUrd_Weapon::OnCollisionEnter(CGameObject* pOther)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(pOther);
	if (false == m_bLanded && (_uint)PlayerState::State_ParrySuccess == pPlayer->Get_NowState())
	{
		m_pTransform->Rotation_RollPitchYaw(0.f, 0.f, 0.f);
		m_pModel->Change_Animation(1, 0.f, false);
		m_bParried = true;
	}
}

HRESULT CUrd_Weapon::Ready_Components(WEAPONDESC* pDesc)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform, nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Attackable"), TEXT("Attackable"), (CComponent**)&m_pAttackable)))
		return E_FAIL;

	if (pDesc->wstrModelTag.size())
	{
		if (FAILED(__super::Add_Component(pDesc->iLevelID, pDesc->wstrModelTag, TEXT("Model"), (CComponent**)&m_pModel)))
			return E_FAIL;
	}

	if (nullptr != pDesc->pColliderDesc)
	{
		wstring wstrColliderTag = L"";
		if (CCollider::OBB == pDesc->pColliderDesc->eType)
			wstrColliderTag = L"Prototype_OBB";
		else if (CCollider::SPHERE == pDesc->pColliderDesc->eType)
			wstrColliderTag = L"Prototype_Sphere";

		pDesc->pColliderDesc->pOwner = this;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, wstrColliderTag, TEXT("Collider"), (CComponent**)&m_pCollider,
			pDesc->pColliderDesc)))
			return E_FAIL;
	}

	return S_OK;
}

void CUrd_Weapon::Init_AttackDesc()
{
	ATTACKDESC AttackDesc{};
	AttackDesc.iDamage = 0;
	AttackDesc.eEnemyAttackType = SEMIKNOCKBACK;

	m_pAttackable->Set_AttackDesc(AttackDesc);

	m_LandingImpactAtkDesc.eEnemyAttackType = KNOCKDOWN;
	m_LandingImpactAtkDesc.iDamage = 100;
}

CUrd_Weapon* CUrd_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUrd_Weapon* pInstance = new CUrd_Weapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUrd_Weapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUrd_Weapon::Clone(void* pArg)
{
	CUrd_Weapon* pInstance = new CUrd_Weapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUrd_Weapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrd_Weapon::Free()
{
	__super::Free();

	Safe_Release(m_pMagicCircle);
	Safe_Release(m_pEffect_SwordDefault);
}
