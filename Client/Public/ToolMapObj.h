#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CToolMapObj final : public CGameObject
{
private:
	CToolMapObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolMapObj(const CToolMapObj& rhs);
	virtual ~CToolMapObj() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	HRESULT Render_Picking(_int iSelectIdx);

	_bool Ray_Cast(_fvector vRayStartPos, _fvector vRayDir, OUT _float4& vPickedPos, OUT _float& fDist);
private:
	CShader*	m_pShader = nullptr;
	CModel*		m_pModel = nullptr;

private:
	HRESULT Ready_Components(void* pArg);

public:
	static CToolMapObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

