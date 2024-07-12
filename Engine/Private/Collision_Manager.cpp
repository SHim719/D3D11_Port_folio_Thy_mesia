#include "Collision_Manager.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"

CCollision_Manager::CCollision_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
}

HRESULT CCollision_Manager::Initialize()
{
	return S_OK;
}

void CCollision_Manager::Update()
{
	Execute_Collision("Player", "Enemy", COLLISION);
	Execute_Collision("Player", "EventTrigger", TRIGGER);
	Execute_Collision("Player", "MapObject", TRIGGER);
	Execute_Collision("Player", "PerceptionBounding", TRIGGER);
	Execute_Collision("Enemy_HitBox", "Player_Weapon", TRIGGER);
	Execute_Collision("Player_HitBox", "Enemy_Weapon", TRIGGER);

}


void CCollision_Manager::Add_ColliderToLayer(const string& strLayer, CCollider* pCollider)
{
	auto it = m_CollisionLayer.find(strLayer);

	if (m_CollisionLayer.end() == it)
	{
		list<CCollider*> Layer;
		Layer.push_back(pCollider);

		m_CollisionLayer.emplace(strLayer, Layer);
	}
	else
		(it->second).push_back(pCollider);

	Safe_AddRef(pCollider);
}


void CCollision_Manager::Execute_Collision(const string& strDstLayer, const string& strSrcLayer, CollisionType eType)
{
	auto DstLayerIt = m_CollisionLayer.find(strDstLayer);
	if (m_CollisionLayer.end() == DstLayerIt)
		return;
	auto SrcLayerIt = m_CollisionLayer.find(strSrcLayer);
	if (m_CollisionLayer.end() == SrcLayerIt)
		return;

	list<CCollider*>& DstColliders = DstLayerIt->second;
	list<CCollider*>& SrcColliders = SrcLayerIt->second;

	for (auto DstIt = DstColliders.begin(); DstIt != DstColliders.end(); )
	{
		CCollider* pDstCollider = *DstIt;
		if (nullptr == pDstCollider)
			continue;

		CGameObject* pDstObj = pDstCollider->Get_Owner();
		if (pDstObj->Is_Destroyed())
		{
			DstIt = DstColliders.erase(DstIt);
			Safe_Release(pDstCollider);
			continue;
		}
			
		for (auto SrcIt = SrcColliders.begin(); SrcIt != SrcColliders.end(); )
		{
			if (*SrcIt == *DstIt)
				continue;

			CCollider* pSrcCollider = *SrcIt;
			if (nullptr == pSrcCollider)
				continue;

			CGameObject* pSrcObj = pSrcCollider->Get_Owner();

			CollisionID id;
			id.left = pDstCollider->Get_CollisionID();
			id.right = pSrcCollider->Get_CollisionID();

			auto it = m_CollisionInfo.find(id.id);
			if (m_CollisionInfo.end() == it)
				m_CollisionInfo.insert({ id.id, false });

			it = m_CollisionInfo.find(id.id);

			if (false == pDstCollider->Is_Active() || false == pSrcCollider->Is_Active() ||
				false == pDstObj->Is_Active() || false == pSrcObj->Is_Active() ||
				pDstObj->Is_OutOfLayer() || pSrcObj->Is_OutOfLayer())
			{
				if (it->second)
				{
					pDstObj->OnCollisionExit(pSrcObj);
					pSrcObj->OnCollisionExit(pDstObj);
			
					pDstCollider->Set_IsColl(false);
					pSrcCollider->Set_IsColl(false);
			
					it->second = false;
				}

				if (pSrcObj->Is_Destroyed())
				{
					SrcIt = SrcColliders.erase(SrcIt);
					Safe_Release(pSrcCollider);
				}
				else
				{
					++SrcIt;
				}
					

				continue;
			}


			if (pSrcCollider->Intersects(pDstCollider))
			{
				if (false == it->second)
				{
					if (COLLISION == eType)
						Push_Object(pDstCollider, pSrcCollider, pDstObj->Get_Transform());
					pDstObj->OnCollisionEnter(pSrcObj);
					pSrcObj->OnCollisionEnter(pDstObj);
					
					it->second = true;
				}

				else
				{
					if (COLLISION == eType)
						Push_Object(pDstCollider, pSrcCollider, pDstObj->Get_Transform());
					pDstObj->OnCollisionStay(pSrcObj);
					pSrcObj->OnCollisionStay(pDstObj);

				}
			}
			else if (it->second)
			{
				pDstObj->OnCollisionExit(pSrcObj);
				pSrcObj->OnCollisionExit(pDstObj);

				it->second = false;
			}

			++SrcIt;
		}

		++DstIt;
	}
	
}

void CCollision_Manager::Push_Object(CCollider* pDstCollider, CCollider* pSrcCollider, CTransform* pDstTransform)
{
	CCollider::ColliderType eDstCollType = pDstCollider->Get_ColliderType();
	CCollider::ColliderType eSrcCollType = pDstCollider->Get_ColliderType();

	if (CCollider::SPHERE == eDstCollType)
	{
		if (CCollider::SPHERE == eSrcCollType)
		{
			BoundingSphere* pDstSphere = static_cast<CSphere*>(pDstCollider)->Get_Collider();
			BoundingSphere* pSrcSphere = static_cast<CSphere*>(pSrcCollider)->Get_Collider();

			_vector vDstCenter = XMLoadFloat3(&(pDstSphere->Center));
			_vector vSrcCenter = XMLoadFloat3(&(pSrcSphere->Center));

			_float vDstRadius = pDstSphere->Radius;
			_float vSrcRadius = pSrcSphere->Radius;

			_vector vPushDir = vDstCenter - vSrcCenter;
			
			_float fDist = vDstRadius + vSrcRadius - XMVectorGetX(XMVector3Length(vPushDir)) - 0.00001f;

			if (fDist > 0.f) {
				vPushDir = XMVector3Normalize(XMVectorSetY(vPushDir, 0.f));

				pDstTransform->Add_Position(vPushDir, fDist);
			}
		}
	}
}

CCollision_Manager* CCollision_Manager::Create()
{
	CCollision_Manager* pInstance = new CCollision_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CCollision_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollision_Manager::Free()
{
	__super::Free();

	for (auto Pair : m_CollisionLayer)
	{
		for (auto pColl : Pair.second)
		{
			Safe_Release(pColl);
		}
	}
}
