#pragma once

#include "Component.h"
#include "DebugDraw.h"

BEGIN(Engine)

class ENGINE_DLL CCollider : public CComponent
{
public:
	enum ColliderType { AABB, OBB, SPHERE, Collider_End };

	typedef struct tagColliderDesc
	{
		_float3				vCenter = { 0.f, 0.f, 0.f };
		_float3				vSize = { 0.f, 0.f, 0.f };
		_float3				vRotation = { 0.f, 0.f, 0.f };
		class CGameObject*	pOwner = nullptr;
		ColliderType		eType = Collider_End;
		_bool				bActive = false;
	}COLLIDERDESC;

protected:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_fmatrix TransformMatrix);

	virtual void Render();

	virtual _bool Intersects(CCollider* pTargetCollider) PURE;
	virtual void Remake_Collider(COLLIDERDESC* pColliderDesc) PURE;
protected:
	class CGameInstance* m_pGameInstance = { nullptr };

protected:
	static _uint		iCollisionID;
	_uint				m_iCollisionID = { 0 };

	ColliderType		m_eColliderType = { Collider_End };

	class CGameObject*	m_pOwner = { nullptr };

	_bool				m_bIsColl = { false };
public:
	_uint Get_CollisionID() {
		return m_iCollisionID;
	}

	ColliderType Get_ColliderType() const { 
		return m_eColliderType; }

	void Set_IsColl(_bool b) { m_bIsColl = b; }

protected:
	PrimitiveBatch<VertexPositionColor>*	m_pBatch = nullptr;
	BasicEffect*							m_pEffect = nullptr;
	ID3D11InputLayout*						m_pInputLayout = nullptr;
	_float4									m_vColor = _float4(0.f, 1.f, 0.f, 1.f);
	
protected:
	_bool m_bActive = true; 

public:
	_bool Is_Active() { return m_bActive; }
	void Set_Active(_bool bActive) { m_bActive = bActive; }
	void Active_Collider() { m_bActive = true; }
	void InActive_Collider() { m_bActive = false; }

protected:
	_matrix Remove_Rotation(_fmatrix Matrix);

public:
	virtual CComponent* Clone(void* pArg) = 0;
	void Free()	override;
};

END

