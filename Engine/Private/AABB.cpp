#include "AABB.h"
#include "OBB.h"
#include "Sphere.h"

CAABB::CAABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCollider(pDevice, pContext)
{
}

CAABB::CAABB(const CAABB& rhs)
	: CCollider(rhs)
	, m_pAABB(rhs.m_pAABB)
{
}

HRESULT CAABB::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_eColliderType = CCollider::AABB;
	return S_OK;
}

HRESULT CAABB::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	COLLIDERDESC* pColliderDesc = (COLLIDERDESC*)pArg;

	m_pOriginal_AABB = new BoundingBox(pColliderDesc->vCenter, _float3(pColliderDesc->vSize.x * 0.5f,
		pColliderDesc->vSize.y * 0.5f,
		pColliderDesc->vSize.z * 0.5f));

	m_pAABB = new BoundingBox(*m_pOriginal_AABB);

	return S_OK;
}

void CAABB::Update(_fmatrix TransformMatrix)
{
	m_pOriginal_AABB->Transform(*m_pAABB, Remove_Rotation(TransformMatrix));
}

_bool CAABB::Intersects(CCollider* pTargetCollider)
{
	CCollider::ColliderType	eType = pTargetCollider->Get_ColliderType();


	switch (eType)
	{
	case CCollider::AABB:
		m_bIsColl = m_pAABB->Intersects(*((CAABB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::OBB:
		m_bIsColl = m_pAABB->Intersects(*((COBB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::SPHERE:
		m_bIsColl = m_pAABB->Intersects(*((CSphere*)pTargetCollider)->Get_Collider());
		break;
	}
	
	return m_bIsColl;
}

void CAABB::Remake_Collider(COLLIDERDESC* pColliderDesc)
{
	Safe_Delete(m_pOriginal_AABB);
	Safe_Delete(m_pAABB);

	m_pOriginal_AABB = new BoundingBox(pColliderDesc->vCenter, _float3(pColliderDesc->vSize.x * 0.5f,
		pColliderDesc->vSize.y * 0.5f,
		pColliderDesc->vSize.z * 0.5f));

	m_pAABB = new BoundingBox(*m_pOriginal_AABB);
}

void CAABB::Render()
{
	__super::Render();

	m_pBatch->Begin();

	DX::Draw(m_pBatch, *m_pAABB, XMLoadFloat4(&m_vColor));

	m_pBatch->End();

}

CAABB* CAABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAABB* pInstance = new CAABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CAABB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CAABB::Clone(void* pArg)
{
	CAABB* pInstance = new CAABB(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CAABB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAABB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginal_AABB);
	Safe_Delete(m_pAABB);
}
