#include "..\Public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;
	m_GameObjects.emplace_back(pGameObject);

	return S_OK;
}

void CLayer::Destroy_Objects()
{
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); )
	{
		if (nullptr != *it)
		{
			if ((*it)->Is_Destroyed())
			{
				Safe_Release(*it);
				it = m_GameObjects.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

void CLayer::PriorityTick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject && pGameObject->Is_Active())
			pGameObject->PriorityTick(fTimeDelta);
	}
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject && pGameObject->Is_Active())
			pGameObject->Tick(fTimeDelta);
	}
}

void CLayer::LateTick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject && pGameObject->Is_Active())
			pGameObject->LateTick(fTimeDelta);
	}
}

CComponent* CLayer::Find_Component(const wstring& strComponentTag, _uint iIndex)
{
	if (iIndex >= m_GameObjects.size())
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	if (iter == m_GameObjects.end())
		return nullptr;

	return (*iter)->Find_Component(strComponentTag);
}

CGameObject* CLayer::Find_GameObject(_uint iIndex)
{
	if (iIndex >= m_GameObjects.size())
		return nullptr;

	auto iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	if (iter == m_GameObjects.end())
		return nullptr;

	return (*iter);
}

CGameObject* CLayer::Find_Target(_fvector vPlayerPos, _fmatrix CameraWorldMatrix)
{
	_float fMinDist = 999999.f;
	CGameObject* pTarget = nullptr;

	_vector vCameraLook = XMVector3Normalize((XMVectorSetY(CameraWorldMatrix.r[2], 0.f)));
	_vector vCameraPos = CameraWorldMatrix.r[3];

	for (auto& pGameObject : m_GameObjects)
	{
		_vector vTargetPos = pGameObject->Get_Transform()->Get_Position();

		_float fDist = XMVector3Length(vTargetPos - vPlayerPos).m128_f32[0];

		if (fDist < fMinDist)
		{
			// 각도 체크(카메라와 타겟이 이루는 각이 둔각일 경우 락온하지않는다.)
			_vector vCameraToTarget = XMVector3Normalize((XMVectorSetY(vTargetPos - vCameraPos, 0.f)));

			if (XMVector3Dot(vCameraLook, vCameraToTarget).m128_f32[0] < 0.f)
				continue;

			fMinDist = fDist;
			pTarget = pGameObject;
		}
	}

	return pTarget;
}

CLayer * CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
	
}
