#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CToolMapObjInstance final : public CGameObject
{
private:
	CToolMapObjInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolMapObjInstance(const CToolMapObjInstance& rhs);
	virtual ~CToolMapObjInstance() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	HRESULT Render()					override;

	void Ready_RenderInstance();
private:
	vector<CTransform*>		m_pInstance_Transforms;

public:
	void Add_Transform(CTransform* pTransform) {
		m_pInstance_Transforms.emplace_back(pTransform);
	}

private:
	CShader*				m_pShader = { nullptr };
	CModel_Instance*		m_pModel_Instance = { nullptr };

public:
	static CToolMapObjInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END

