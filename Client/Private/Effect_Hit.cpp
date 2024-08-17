#include "Effect_Hit.h"
#include "GameInstance.h"

#include "GameEffect.h"
#include "Effect_Group.h"


CEffect_Hit::CEffect_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffect_Hit::CEffect_Hit(const CEffect_Hit& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_Hit::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Hit::Initialize(void* pArg)
{
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	EFFECT_HITDESC* pDesc = (EFFECT_HITDESC*)pArg;

	m_iTag = pDesc->iTag;

	if (nullptr == pDesc->EffectSpawnDesc.pParentTransform)
		pDesc->EffectSpawnDesc.pParentTransform = m_pTransform;

	m_pEffect_Group = pDesc->pEffect_Group;

	m_pTransform->Set_Position(XMLoadFloat4(&pDesc->vSpawnPos));

	Safe_AddRef(m_pEffect_Group);
	if (FAILED(m_pEffect_Group->OnEnter_Layer(&pDesc->EffectSpawnDesc)))
		return E_FAIL;

	m_pGameInstance->Insert_GameObject(GET_CURLEVEL, L"Effect", m_pEffect_Group);
	
	return S_OK;
}

void CEffect_Hit::Tick(_float fTimeDelta)
{
	if (false == m_pEffect_Group->Is_Using())
		Set_Destroy(true);
}

void CEffect_Hit::LateTick(_float fTimeDelta)
{
	m_pCollider->Update(m_pTransform->Get_WorldMatrix());

#ifdef _DEBUG
	m_pGameInstance->Add_RenderComponent(m_pCollider);
#endif

}


HRESULT CEffect_Hit::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Attackable"), TEXT("Attackable"), (CComponent**)&m_pAttackable)))
		return E_FAIL;

	EFFECT_HITDESC* pDesc = (EFFECT_HITDESC*)pArg;

	if (nullptr != pDesc->pColliderDesc)
	{
		wstring wstrColliderTag = L"";

		if (CCollider::OBB == pDesc->pColliderDesc->eType)
			wstrColliderTag = L"Prototype_OBB";
		else if (CCollider::SPHERE == pDesc->pColliderDesc->eType)
			wstrColliderTag = L"Prototype_Sphere";
		else
			wstrColliderTag = L"Prototype_AABB";

		pDesc->pColliderDesc->pOwner = this;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, wstrColliderTag, TEXT("Collider"), (CComponent**)&m_pCollider,
			pDesc->pColliderDesc)))
			return E_FAIL;
	}

	m_pAttackable->Set_AttackDesc(pDesc->AttackDesc);

	return S_OK;
}

CEffect_Hit* CEffect_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Hit* pInstance = new CEffect_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Hit::Clone(void* pArg)
{
	CEffect_Hit* pInstance = new CEffect_Hit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEffect_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Hit::Free()
{
	__super::Free();

	Safe_Release(m_pCollider);
	Safe_Release(m_pAttackable);
	Safe_Release(m_pEffect_Group);
}
