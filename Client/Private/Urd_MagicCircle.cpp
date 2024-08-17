#include "Urd_MagicCircle.h"
#include "GameInstance.h"

#include "GameEffect.h"
#include "Effect_Group.h"
#include "Effect_Manager.h"


CUrd_MagicCircle::CUrd_MagicCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUrd_MagicCircle::CUrd_MagicCircle(const CUrd_MagicCircle& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUrd_MagicCircle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUrd_MagicCircle::Initialize(void* pArg)
{
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_iTag = TAG_ENEMY_WEAPON;

	m_tEffectSpawnDesc.pParentTransform = m_pTransform;

	m_pEffect_MagicAppear = EFFECTMGR->Get_EffectGroup("Effect_Urd_MagicCircle_Appear");
	if (nullptr == m_pEffect_MagicAppear)
		assert(false);
	m_pEffect_MagicAppear->Set_Using(true);
	Safe_AddRef(m_pEffect_MagicAppear);

	m_pEffect_MagicImpact = EFFECTMGR->Get_EffectGroup("Effect_Urd_MagicCircle_Impact");
	if (nullptr == m_pEffect_MagicImpact)
		assert(false);
	m_pEffect_MagicImpact->Set_Using(true);
	Safe_AddRef(m_pEffect_MagicImpact);

	return S_OK;
}

void CUrd_MagicCircle::Tick(_float fTimeDelta)
{
	m_pNowEffect->Tick(fTimeDelta);
	if (false == m_bUltimate && false == m_bExplosion)
	{
		m_fAppearingTime -= fTimeDelta;
		if (m_fAppearingTime < 0.f)
			Explosion();
	}

	if (true == m_bExplosion && false == m_pEffect_MagicImpact->Is_Using())
		m_bReturnToPool = true;
	
}

void CUrd_MagicCircle::LateTick(_float fTimeDelta)
{
	m_pNowEffect->LateTick(fTimeDelta);
#ifdef _DEBUG
	m_pGameInstance->Add_RenderComponent(m_pCollider);
#endif

}

HRESULT CUrd_MagicCircle::OnEnter_Layer(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_bReturnToPool = false; 
	URD_MAGICCIRCLE_DESC* pDesc = (URD_MAGICCIRCLE_DESC*)pArg;
	m_pTransform->Set_Position(XMLoadFloat4(&pDesc->vPosition));
	m_bUltimate = pDesc->bUltimate;

	m_bExplosion = false;
	m_pCollider->Set_Active(false);

	Change_Effect(m_pEffect_MagicAppear);

	m_fAppearingTime = 1.5f;
	
	return S_OK;
}

void CUrd_MagicCircle::Explosion()
{
	m_bExplosion = true;
	Change_Effect(m_pEffect_MagicImpact);
	m_pCollider->Set_Active(true);
	m_pCollider->Update(m_pTransform->Get_WorldMatrix());

}

void CUrd_MagicCircle::Change_Effect(CEffect_Group* pEffect)
{
	m_pNowEffect = pEffect;
	m_pNowEffect->Start_Effect(&m_tEffectSpawnDesc);
	m_pNowEffect->Set_Using(true);
}


HRESULT CUrd_MagicCircle::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Attackable"), TEXT("Attackable"), (CComponent**)&m_pAttackable)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc;
	ColliderDesc.eType = CCollider::SPHERE;
	ColliderDesc.strCollisionLayer = "Enemy_Weapon";
	ColliderDesc.vSize = { 10.f, 1.f, 1.f };
	ColliderDesc.pOwner = this;
	ColliderDesc.bActive = false;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Sphere", TEXT("Collider"), (CComponent**)&m_pCollider, &ColliderDesc)))
		return E_FAIL;


	ATTACKDESC AttackDesc;
	AttackDesc.eEnemyAttackType = AIR;
	AttackDesc.iDamage = 122;

	m_pAttackable->Set_AttackDesc(AttackDesc);

	return S_OK;
}

CUrd_MagicCircle* CUrd_MagicCircle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUrd_MagicCircle* pInstance = new CUrd_MagicCircle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUrd_MagicCircle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUrd_MagicCircle::Clone(void* pArg)
{
	CUrd_MagicCircle* pInstance = new CUrd_MagicCircle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUrd_MagicCircle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrd_MagicCircle::Free()
{
	__super::Free();

	Safe_Release(m_pCollider);
	Safe_Release(m_pAttackable);
	Safe_Release(m_pEffect_MagicAppear);
	Safe_Release(m_pEffect_MagicImpact);
}				
