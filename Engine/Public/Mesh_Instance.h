#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CMesh_Instance final : public CVIBuffer_Instance
{
private:
	CMesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh_Instance(const CMesh_Instance& rhs);
	virtual ~CMesh_Instance() = default;

public:
	HRESULT Initialize_Prototype(ifstream& fin);
	HRESULT Initialize(void* pArg) override;

	HRESULT Update_InstanceBuffer(vector<class CTransform*>& InstanceTransforms);

private:
	VTXMODEL*		m_pModelVertices = { nullptr };
	VTXMATRIX*		m_pInstanceVertices = { nullptr };
	FACEINDICES32*	m_pIndices = { nullptr };

private:
	void Culling_Frustum(vector<class CTransform*>& InstanceTransforms, OUT vector<CTransform*>& InstanceDatas);

private:
	HRESULT Ready_Vertices(ifstream& fin);
	HRESULT Ready_IndexBuffer(ifstream& fin);

	HRESULT Init_InstanceBuffer(_uint iInstanceCount);
	
public:
	static CMesh_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& fin);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END