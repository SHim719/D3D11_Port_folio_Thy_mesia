#include "Collision_Manager.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"

CCollision_Manager::CCollision_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCollision_Manager::Initialize()
{
	return S_OK;
}

void CCollision_Manager::Update()
{
	Execute_Collision(m_pGameInstance->Get_CurrentLevelID(), L"Player", L"Enemy", CollisionType::Collision);

}


void CCollision_Manager::Execute_Collision(_uint iLevel, const wstring& strDstLayer, const wstring& strSrcLayer, CollisionType eCollisionType)
{
	CLayer* pDstLayer = m_pGameInstance->Find_Layer(iLevel, strDstLayer);
	CLayer* pSrcLayer = m_pGameInstance->Find_Layer(iLevel, strSrcLayer);
	
	if (nullptr == pDstLayer || nullptr == pSrcLayer)
		return;
	
	auto& DstObjects = pDstLayer->Get_GameObjects();
	auto& SrcObjects = pSrcLayer->Get_GameObjects();
	
	for (auto DstIt = DstObjects.begin(); DstIt != DstObjects.end(); ++DstIt)
	{
		if ((*DstIt)->Is_Destroyed() || false == (*DstIt)->Is_Active())
			continue;
		CCollider* pDstCollider = static_cast<CCollider*>((*DstIt)->Find_Component(L"Collider"));

		if (nullptr == pDstCollider || false == pDstCollider->Is_Active())
			continue;

		for (auto SrcIt = SrcObjects.begin(); SrcIt != SrcObjects.end(); ++SrcIt)
		{
			if (*SrcIt == *DstIt || (*SrcIt)->Is_Destroyed() || false == (*SrcIt)->Is_Active())
				continue;
	
			CCollider* pSrcCollider = static_cast<CCollider*>((*SrcIt)->Find_Component(L"Collider"));
			if (nullptr == pSrcCollider || false == pSrcCollider->Is_Active())
				continue;
	
			CollisionID id;
			id.left = pDstCollider->Get_CollisionID();
			id.right = pSrcCollider->Get_CollisionID();
	
			auto it = m_CollisionInfo.find(id.id);
			if (m_CollisionInfo.end() == it)
				m_CollisionInfo.insert({ id.id, false });
	
			it = m_CollisionInfo.find(id.id);
	
			_float3 fDist;
			if (pSrcCollider->Intersects(pDstCollider))
			{
				if (false == it->second)
				{
					if (Trigger == eCollisionType)
					{
						(*DstIt)->OnTriggerEnter(*SrcIt);
						(*SrcIt)->OnTriggerEnter(*DstIt);
					}

					else
					{
						(*DstIt)->OnCollisionEnter(*SrcIt);
						(*SrcIt)->OnCollisionEnter(*DstIt);
					}
					it->second = true;
				}

				else
				{
					if (Trigger == eCollisionType)
					{
						(*DstIt)->OnTriggerStay(*SrcIt);
						(*SrcIt)->OnTriggerStay(*DstIt);
					}
					else
					{

						(*DstIt)->OnCollisionStay(*SrcIt);
						(*SrcIt)->OnCollisionStay(*DstIt);
					}
				}
			}
			else if (it->second)
			{
				if (Trigger == eCollisionType)
				{
					(*DstIt)->OnTriggerExit(*SrcIt);
					(*SrcIt)->OnTriggerExit(*DstIt);
				}
				else
				{
					(*DstIt)->OnCollisionExit(*SrcIt);
					(*SrcIt)->OnCollisionExit(*DstIt);
				}
				it->second = false;
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

	Safe_Release(m_pGameInstance);
}
