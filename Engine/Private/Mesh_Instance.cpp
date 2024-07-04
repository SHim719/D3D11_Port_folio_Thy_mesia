#include "Mesh_Instance.h"
#include "GameInstance.h"


CMesh_Instance::CMesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance{ pDevice, pContext }
{
}

CMesh_Instance::CMesh_Instance(const CMesh_Instance& rhs)
	: CVIBuffer_Instance(rhs)
	, m_pInstanceVertices(rhs.m_pInstanceVertices)
{
}

HRESULT CMesh_Instance::Initialize_Prototype(ifstream& fin)
{
	if (FAILED(Ready_Vertices(fin)))
		return E_FAIL;

	if (FAILED(Ready_IndexBuffer(fin)))
		return E_FAIL;

	m_iInstanceVertexStride = sizeof(VTXMATRIX);

	m_iNumIndexPerInstance = m_iNumPrimitives * m_iNumIndicesofPrimitive;

	return S_OK;
}

HRESULT CMesh_Instance::Initialize(void* pArg)
{

	return S_OK;
}

HRESULT CMesh_Instance::Init_InstanceBuffer(_uint iInstanceCount)
{
	if (iInstanceCount == m_iNumInstance)
		return S_OK;

	m_iNumInstance = iInstanceCount;

	Safe_Release(m_pVBInstance);

	m_pInstanceVertices = new VTXMATRIX[m_iNumInstance];

	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
	m_BufferDesc.ByteWidth = m_iInstanceVertexStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceVertexStride;
	
	ZeroMemory(&m_SubResourceData, sizeof(m_SubResourceData));
	m_SubResourceData.pSysMem = m_pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(m_pInstanceVertices);
	
	return S_OK;
}

HRESULT CMesh_Instance::Update_InstanceBuffer(vector<CTransform*>& InstanceTransforms)
{
	// ÄÃ¸µ
	vector<CTransform*> InstanceDatas;
	InstanceDatas.reserve(InstanceTransforms.size());

	Culling_Frustum(InstanceTransforms, InstanceDatas);

	if (FAILED(Init_InstanceBuffer((_uint)InstanceDatas.size())))
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	for (size_t i = 0; i < InstanceDatas.size(); ++i)
	{
		_matrix WorldMatrix = InstanceDatas[i]->Get_WorldMatrix();

		XMStoreFloat4(&(static_cast<VTXMATRIX*>(SubResource.pData)[i].vRight), WorldMatrix.r[0]);
		XMStoreFloat4(&(static_cast<VTXMATRIX*>(SubResource.pData)[i].vUp), WorldMatrix.r[1]);
		XMStoreFloat4(&(static_cast<VTXMATRIX*>(SubResource.pData)[i].vLook), WorldMatrix.r[2]);
		XMStoreFloat4(&(static_cast<VTXMATRIX*>(SubResource.pData)[i].vTranslation), WorldMatrix.r[3]);

	}

	m_pContext->Unmap(m_pVBInstance, 0);

	return S_OK;
}


void CMesh_Instance::Culling_Frustum(vector<CTransform*>& InstanceTransforms, OUT vector<CTransform*>& InstanceDatas)
{
	for (auto& pTransform : InstanceTransforms)
	{
		// Culling
		InstanceDatas.emplace_back(pTransform);
	}

}

HRESULT CMesh_Instance::Ready_Vertices(ifstream& fin)
{
	fin.read((char*)&m_iNumVertices, sizeof(_uint));

	m_iNumVertexBuffers = 2;
	m_iStride = sizeof(VTXMODEL);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	m_pModelVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(m_pModelVertices, sizeof(VTXMODEL) * m_iNumVertices);

	fin.read((char*)m_pModelVertices, sizeof(VTXMODEL) * m_iNumVertices);

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = m_pModelVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

#ifndef _DEBUG
	Safe_Delete_Array(m_pModelVertices);
#endif

	return S_OK;
}

HRESULT CMesh_Instance::Ready_IndexBuffer(ifstream& fin)
{
	fin.read((char*)&m_iNumPrimitives, sizeof(_uint));

	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	m_pIndices = new FACEINDICES32[m_iNumPrimitives];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);
	fin.read((char*)m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = m_pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

#ifndef _DEBUG
	Safe_Delete_Array(m_pIndices);
#endif

	return S_OK;
}

CMesh_Instance* CMesh_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& fin)
{
	CMesh_Instance* pInstance = new CMesh_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(fin)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CMesh_Instance::Clone(void* pArg)
{
	CMesh_Instance* pInstance = new CMesh_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh_Instance::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pModelVertices);
	Safe_Delete_Array(m_pInstanceVertices);
	Safe_Delete_Array(m_pIndices);

}
