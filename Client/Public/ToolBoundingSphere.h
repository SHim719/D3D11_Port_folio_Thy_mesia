#pragma once

#include "Client_Defines.h"
#include "ToolMapObj.h"

BEGIN(Client)
class CToolBoundingSphere final : public CToolMapObj
{
private:
	CToolBoundingSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolBoundingSphere(const CToolBoundingSphere& rhs);
	virtual ~CToolBoundingSphere() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;

private:
	HRESULT Ready_Components();

public:
	static CToolBoundingSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END

