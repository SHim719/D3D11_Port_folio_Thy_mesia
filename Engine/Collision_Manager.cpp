#include "Collision_Manager.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "BoxCollider.h"


CCollision_Manager::CCollision_Manager()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCollision_Manager::Initialize()
{
	return S_OK;
}

void CCollision_Manager::Tick()
{

	Intersect_Ray();
}

void CCollision_Manager::Intersect_Ray()
{
	for (auto& RayDesc : m_RayDescs)
	{
		CLayer* pLayer = m_pGameInstance->Find_Layer(RayDesc.iLevel, RayDesc.strDstLayer);
		if (pLayer == nullptr)
			continue;

		auto& gameObjects = pLayer->Get_GameObjects();

		for (auto it = gameObjects.begin(); it != gameObjects.end(); ++it)
		{
			if (false == (*it)->Is_Active())
				continue;

			if (!(*it)->Can_Intersect())
				continue;

			if (nullptr == pVIBuffer)
				continue;

			_float3 fHitWorldPos;
			_float fDist;
			if (pVIBuffer->Intersect_Ray((*it)->Get_Transform(), RayDesc.vRayWorldPos, RayDesc.vRayDir, &fHitWorldPos, &fDist))
			{
				if ((*it)->On_Ray_Intersect(fHitWorldPos, fDist, RayDesc.pArg))
					break;
			}
		}
	}

	m_RayDescs.clear();
}

_bool CCollision_Manager::Ray_Cast(const RAY_DESC& RayDesc, OUT CGameObject*& pOutHit, OUT _float3& fHitWorldPos, OUT _float& fDist)
{
	CLayer* pLayer = m_pGameInstance->Find_Layer(RayDesc.iLevel, RayDesc.strDstLayer);
	if (pLayer == nullptr)
		return false;

	auto& gameObjects = pLayer->Get_GameObjects();


	for (auto it = gameObjects.begin(); it != gameObjects.end(); ++it)
	{
		if (false == (*it)->Is_Active())
			continue;

		CVIBuffer* pVIBuffer = dynamic_cast<CVIBuffer*>((*it)->Find_Component(L"VIBuffer"));
		if (nullptr == pVIBuffer)
			continue;
		if (!(*it)->Can_Intersect())
			continue;

		_float3 _fHitWorldPos;
		_float _fDist;
		if (pVIBuffer->Intersect_Ray((*it)->Get_Transform(), RayDesc.vRayWorldPos, RayDesc.vRayDir, &_fHitWorldPos, &_fDist))
		{
			pOutHit = *it;
			fHitWorldPos = _fHitWorldPos;
			fDist = _fDist;
			assert(fDist >= 0.f);
			return true;
		}
	}

	return false;
}

void CCollision_Manager::Collision_Box(_uint iLevel, const wstring& strDstLayer, const wstring& strSrcLayer, CollisionType eCollisionType)
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
		CTransform* pDstTransform = (*DstIt)->Get_Transform();
		CBoxCollider* pDstCollider = dynamic_cast<CBoxCollider*>((*DstIt)->Find_Component(L"Collider"));
		if (nullptr == pDstCollider || false == pDstCollider->IsActive())
			continue;

		for (auto SrcIt = SrcObjects.begin(); SrcIt != SrcObjects.end(); ++SrcIt)
		{
			if (*SrcIt == *DstIt || (*SrcIt)->Is_Destroyed() || false == (*SrcIt)->Is_Active())
				continue;

			CTransform* pSrcTransform = (*SrcIt)->Get_Transform();
			CBoxCollider* pSrcCollider = dynamic_cast<CBoxCollider*>((*SrcIt)->Find_Component(L"Collider"));
			if (nullptr == pSrcCollider || false == pSrcCollider->IsActive())
				continue;

			CollisionID id;
			id.left = pDstCollider->Get_CollisionID();
			id.right = pSrcCollider->Get_CollisionID();

			auto it = m_CollisionInfo.find(id.id);
			if (m_CollisionInfo.end() == it)
				m_CollisionInfo.insert({ id.id, false });

			it = m_CollisionInfo.find(id.id);

			_float3 fDist;
			if (Check_Intersect_AABB(pDstCollider, pSrcCollider, fDist))
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
						pDstTransform->Add_Pos(fDist);
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
						pDstTransform->Add_Pos(fDist);
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

bool CCollision_Manager::Check_Intersect_AABB(CBoxCollider* pDstCollider, CBoxCollider* pSrcCollider, OUT _float3& _fDist)
{
	_float3 vDstMin = pDstCollider->Get_MinCoord();
	_float3 vDstMax = pDstCollider->Get_MaxCoord();

	_float3 vSrcMin = pSrcCollider->Get_MinCoord();
	_float3 vSrcMax = pSrcCollider->Get_MaxCoord();

	if (min(vDstMax.x, vSrcMax.x) < max(vDstMin.x, vSrcMin.x))
		return false;
	if (min(vDstMax.y, vSrcMax.y) < max(vDstMin.y, vSrcMin.y))
		return false;
	if (min(vDstMax.z, vSrcMax.z) < max(vDstMin.z, vSrcMin.z))
		return false;

	_float3 vDstCenter = (vDstMin + vDstMax) * 0.5f;
	_float3 vSrcCenter = (vSrcMin + vSrcMax) * 0.5f;


	if (vDstCenter.x < vSrcCenter.x)
		_fDist.x = -(min(vSrcMax.x, vDstMax.x) - max(vSrcMin.x, vDstMin.x));
	else
		_fDist.x = (min(vSrcMax.x, vDstMax.x) - max(vSrcMin.x, vDstMin.x));

	if (vDstCenter.y < vSrcCenter.y)
		_fDist.y = -(min(vSrcMax.y, vDstMax.y) - max(vSrcMin.y, vDstMin.y));
	else
		_fDist.y = (min(vSrcMax.y, vDstMax.y) - max(vSrcMin.y, vDstMin.y));

	if (vDstCenter.z < vSrcCenter.z)
		_fDist.z = -(min(vSrcMax.z, vDstMax.z) - max(vSrcMin.z, vDstMin.z));
	else
		_fDist.z = (min(vSrcMax.z, vDstMax.z) - max(vSrcMin.z, vDstMin.z));

	if (fabsf(_fDist.x) <= fabsf(_fDist.y))
	{
		_fDist.y = 0.f;
		if (fabsf(_fDist.x) <= fabsf(_fDist.z))
			_fDist.z = 0.f;

		else
			_fDist.x = 0.f;
	}
	else
	{
		_fDist.x = 0.f;
		if (fabsf(_fDist.y) <= fabsf(_fDist.z))
			_fDist.z = 0.f;

		else
			_fDist.y = 0.f;
	}


	return true;
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
