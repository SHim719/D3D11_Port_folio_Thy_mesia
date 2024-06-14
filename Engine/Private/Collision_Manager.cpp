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
	Execute_Collision(m_pGameInstance->Get_CurrentLevelID(), L"Player", L"Enemy", COLLISION);
	Execute_Collision(m_pGameInstance->Get_CurrentLevelID(), L"Player_Weapon", L"Enemy", TRIGGER);
	Execute_Collision(m_pGameInstance->Get_CurrentLevelID(), L"Enemy_Weapon", L"Player", TRIGGER);

}


void CCollision_Manager::Execute_Collision(_uint iLevel, const wstring& strDstLayer, const wstring& strSrcLayer, CollisionType eType)
{
	if (iLevel < 2)
		return;

	CLayer* pDstLayer = m_pGameInstance->Find_Layer(iLevel, strDstLayer);
	CLayer* pSrcLayer = m_pGameInstance->Find_Layer(iLevel, strSrcLayer);
	
	if (nullptr == pDstLayer || nullptr == pSrcLayer)
		return;
	
	auto& DstObjects = pDstLayer->Get_GameObjects();
	auto& SrcObjects = pSrcLayer->Get_GameObjects();
	
	for (auto DstIt = DstObjects.begin(); DstIt != DstObjects.end(); ++DstIt)
	{
		CCollider* pDstCollider = static_cast<CCollider*>((*DstIt)->Find_Component(L"Collider"));

		if (nullptr == pDstCollider)
			continue;

		for (auto SrcIt = SrcObjects.begin(); SrcIt != SrcObjects.end(); ++SrcIt)
		{
			if (*SrcIt == *DstIt)
				continue;
	
			CCollider* pSrcCollider = static_cast<CCollider*>((*SrcIt)->Find_Component(L"Collider"));
			if (nullptr == pSrcCollider)
				continue;
	
			CollisionID id;
			id.left = pDstCollider->Get_CollisionID();
			id.right = pSrcCollider->Get_CollisionID();
	
			auto it = m_CollisionInfo.find(id.id);
			if (m_CollisionInfo.end() == it)
				m_CollisionInfo.insert({ id.id, false });
	
			it = m_CollisionInfo.find(id.id);
	
			if (false == (*DstIt)->Is_Active() || false == pDstCollider->Is_Active() || (*DstIt)->Is_Destroyed() ||
				(*SrcIt)->Is_Destroyed() || false == (*SrcIt)->Is_Active() || false == pSrcCollider->Is_Active())
			{
				if (it->second)
				{
					(*DstIt)->OnCollisionExit(*SrcIt);
					(*SrcIt)->OnCollisionExit(*DstIt);

					pDstCollider->Set_IsColl(false);
					pSrcCollider->Set_IsColl(false);

					it->second = false;
				}
				continue;
			}


			if (pSrcCollider->Intersects(pDstCollider))
			{
				if (false == it->second)
				{
					if (COLLISION == eType) 
						Push_Object(pDstCollider, pSrcCollider, (*DstIt)->Get_Transform());
					(*DstIt)->OnCollisionEnter(*SrcIt);
					(*SrcIt)->OnCollisionEnter(*DstIt);
					
					it->second = true;
				}

				else
				{
					if (COLLISION == eType) 
						Push_Object(pDstCollider, pSrcCollider, (*DstIt)->Get_Transform());
					(*DstIt)->OnCollisionStay(*SrcIt);
					(*SrcIt)->OnCollisionStay(*DstIt);
					
				}
			}
			else if (it->second)
			{
				(*DstIt)->OnCollisionExit(*SrcIt);
				(*SrcIt)->OnCollisionExit(*DstIt);
				
				it->second = false;
			}
		}
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
			_float vSrcRadius = pDstSphere->Radius;

			_vector vPushDir = vDstCenter - vSrcCenter;
			
			_float fDist = vDstRadius + vSrcRadius - XMVectorGetX(XMVector3Length(vPushDir));

			vPushDir = XMVector3Normalize(XMVectorSetY(vPushDir, 0.f));

			pDstTransform->Add_Position(vPushDir, fDist);
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
}
