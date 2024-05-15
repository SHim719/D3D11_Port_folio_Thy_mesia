#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider abstract : public CComponent
{
protected:
	CCollider(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual void Render();
protected:
	class CGameInstance* m_pGameInstance = { nullptr };
	_float3		m_vOffset;

public:
	void Set_Offset(const _float3& vOffset) { m_vOffset = vOffset; }
	const _float3& Get_Offset() { return m_vOffset; }

protected:
	static _uint	iCollisionID;
	_uint	m_iCollisionID = { 0 };

public:
	_uint Get_CollisionID() { return m_iCollisionID; }

private:
	_bool m_bActive = { true }; // 활성화 되어있느냐

public:
	_bool IsActive() { return m_bActive; }
	void Set_Active(_bool bActive) { m_bActive = bActive; }

public:
	virtual CComponent* Clone(void* pArg) = 0;
	void Free()	override;
};

END

