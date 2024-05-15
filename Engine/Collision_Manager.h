#pragma once

#include "Base.h"
#include "Collider.h"


BEGIN(Engine)
class CCollision_Manager : public CBase
{
private:
	union CollisionID
	{
		struct
		{
			UINT32 left;
			UINT32 right;
		};

		UINT64 id;
	};

private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

	HRESULT Initialize();

public:
	void Tick();

private:
	unordered_map<UINT64, bool> m_CollisionInfo;

private:
	void Collision_Box(_uint iLevel, const wstring& strDstLayer, const wstring& strSrcLayer, CollisionType eCollisionType);

	bool Check_Intersect_AABB(class CBoxCollider* pDstCollider, CBoxCollider* pSrcCollider, OUT _float3& _fDist);

private:
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	static CCollision_Manager* Create();
	void Free() override;

};

END

