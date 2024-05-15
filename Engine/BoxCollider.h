#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CBoxCollider final : public CCollider
{
public:
	typedef struct tagBoxCollisionDesc
	{
		_float3 vScale = { 1.f, 1.f, 1.f };
		_float3 vOffset = { 0.f, 0.f, 0.f };
	} BOXCOLLISION_DESC;

protected:
	CBoxCollider(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoxCollider(const CBoxCollider& rhs);
	virtual ~CBoxCollider() = default;

public:
	HRESULT Initialize_Prototype()	override;
	HRESULT Initialize(void* pArg)	override;

	void	Render()	override;

public:
	void	Update_BoxCollider(const _float4x4& Owner_WorldMatrix);

private:
	_float4x4	m_CollisionMatrix = {};
	_float3		m_vMin = {};
	_float3		m_vMax = {};

public:
	_float3		Get_MinCoord() { return m_vMin; }
	_float3		Get_MaxCoord() { return m_vMax; }

public:
	void Set_Scale(const _float3& vScale);
	_float3 Get_Scale();

private:
	class CVIBuffer_Box* m_pVIBuffer_Box = { nullptr };

public:
	static CBoxCollider* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CComponent* Clone(void* pArg)	override;
	void Free()	override;
};

END