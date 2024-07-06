#include "ToolBoundingSphere.h"

CToolBoundingSphere::CToolBoundingSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CToolMapObj(pDevice, pContext)
{
}

CToolBoundingSphere::CToolBoundingSphere(const CToolBoundingSphere& rhs)
	: CToolMapObj(rhs)
{

}

HRESULT CToolBoundingSphere::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CToolBoundingSphere::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}


HRESULT CToolBoundingSphere::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	CCollider::COLLIDERDESC CollisionDesc;
	CollisionDesc.eType = CCollider::SPHERE;
	CollisionDesc.pOwner = this;
	CollisionDesc.vCenter = { 0.f, 0.f, 0.f };
	CollisionDesc.vSize = { 1.f, 1.f, 1.f };
	CollisionDesc.vRotation = { 0.f, 0.f, 0.f };
	CollisionDesc.bActive = true;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Sphere", L"Collider", (CComponent**)&m_pTriggerCollider, &CollisionDesc)))
		return E_FAIL;

	return S_OK;
}


CToolBoundingSphere* CToolBoundingSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolBoundingSphere* pInstance = new CToolBoundingSphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CToolBoundingSphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToolBoundingSphere::Clone(void* pArg)
{
	CToolBoundingSphere* pInstance = new CToolBoundingSphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CToolBoundingSphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolBoundingSphere::Free()
{
	__super::Free();
}
