#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CSphere final : public CCollider
{
protected:
	CSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSphere(const CSphere& rhs);
	virtual ~CSphere() = default;

	_bool Intersects(CCollider* pTargetCollider)		override;
	void Remake_Collider(COLLIDERDESC* pColliderDesc)	override;
public:
	BoundingSphere* Get_Collider() {
		return m_pSphere;
	}

public:
	HRESULT Initialize_Prototype()	override;
	HRESULT Initialize(void* pArg)								override;
	void Update(_fmatrix TransformMatrix)						override;


#ifdef _DEBUG
public:
	HRESULT Render() override;
#endif 

private:
	BoundingSphere* m_pSphere = nullptr;
	BoundingSphere* m_pOriginal_Sphere = nullptr;


public:
	static CSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent* Clone(void* pArg) override;
	void Free() override;


};

END