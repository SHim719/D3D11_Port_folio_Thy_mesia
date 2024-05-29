#include "..\Public\MeshContainer.h"
#include "Model.h"
#include "Bone.h"

CMeshContainer::CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMeshContainer::CMeshContainer(const CMeshContainer& rhs)
	: CVIBuffer(rhs)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
{
	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CMeshContainer::Initialize(ifstream& fin, CModel::TYPE eType)
{
	fin.read((char*)&m_iMaterialIndex, sizeof(_uint));
	fin.read((char*)&m_iNumVertices, sizeof(_uint));

	if (CModel::TYPE_NONANIM == eType)
		Ready_Vertices(fin);
	else
		Ready_AnimVertices(fin);

#pragma region IndexBuffer
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

#pragma endregion 

	return S_OK;
}


HRESULT CMeshContainer::Ready_Vertices(ifstream& fin)
{
	m_iNumVertexBuffers = 1;
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

	return S_OK;
}

HRESULT CMeshContainer::Ready_AnimVertices(ifstream& fin)
{
	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXANIMMODEL);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	m_pAnimVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(m_pAnimVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	fin.read((char*)m_pAnimVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = m_pAnimVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	return S_OK;

}

CMeshContainer* CMeshContainer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& fin, CModel::TYPE eModelType)
{
	CMeshContainer* pInstance = new CMeshContainer(pDevice, pContext);

	if (FAILED(pInstance->Initialize(fin, eModelType)))
	{
		MSG_BOX(TEXT("Failed To Created : CMeshContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMeshContainer::Clone(void* pArg)
{
	return nullptr;
}

void CMeshContainer::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pModelVertices);
	Safe_Delete_Array(m_pAnimVertices);
	Safe_Delete_Array(m_pIndices);
}
