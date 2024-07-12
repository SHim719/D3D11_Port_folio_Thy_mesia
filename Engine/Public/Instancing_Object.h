#pragma once

#include "GameObject.h"

BEGIN(Engine)
class ENGINE_DLL CInstancing_Object final : public CGameObject
{
private:
	CInstancing_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstancing_Object(const CInstancing_Object& rhs);
	virtual ~CInstancing_Object() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	vector<_float4x4>		m_WorldMatrices;

public:
	void Add_WorldMatrix(const _float4x4& WorldMatrix) {
		m_WorldMatrices.emplace_back(WorldMatrix);
	}

private:
	CShader*			m_pShader = { nullptr };
	CModel_Instance*	m_pModel_Instance = { nullptr };

public:
	static CInstancing_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END

