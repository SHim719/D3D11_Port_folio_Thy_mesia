#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL COBB final : public CCollider
{
protected:
	COBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COBB(const COBB& rhs);
	virtual ~COBB() = default;

	_bool Intersects(CCollider* pTargetCollider)		override;
	void Remake_Collider(COLLIDERDESC* pColliderDesc)	override;
public:
	BoundingOrientedBox* Get_Collider() {
		return m_pOBB;
	}

public:
	HRESULT Initialize_Prototype()	override;
	HRESULT Initialize(void* pArg)								override;
	void Update(_fmatrix TransformMatrix)						override;
	
														
public:
	HRESULT Render() override;


private:
	BoundingOrientedBox* m_pOBB = nullptr;
	BoundingOrientedBox* m_pOriginal_OBB = nullptr;


public:
	static COBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	 CComponent* Clone(void* pArg) override;
	 void Free() override;


};

END