#include "..\Public\VIBuffer_Trail.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& rhs)
	: CVIBuffer(rhs)

{

}

HRESULT CVIBuffer_Trail::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	TRAIL_DESC* pTrailDesc = (TRAIL_DESC*)pArg;
	m_tTrailDesc = *pTrailDesc;

	if (FAILED(Init_Buffers()))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Trail::Update(_matrix ParentMatrix)
{
	D3D11_MAPPED_SUBRESOURCE SubResource;

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXTEX* pVertices = (VTXTEX*)SubResource.pData;

	_vector vStartPos = XMVector3TransformCoord(XMLoadFloat3(&m_tTrailDesc.vStartPos), ParentMatrix);
	_vector vEndPos = XMVector3TransformCoord(XMLoadFloat3(&m_tTrailDesc.vEndPos), ParentMatrix);

	if (m_iNowVertexCount >= m_iNumVertices)
	{
		_uint iRemoveCount = m_tTrailDesc.iRemoveCount;
		m_iNowVertexCount -= iRemoveCount;

		for (_uint i = 0; i < m_iNowVertexCount; i += 2)
		{
			pVertices[i].vPosition = pVertices[iRemoveCount + i].vPosition;
			pVertices[i + 1].vPosition = pVertices[iRemoveCount + i + 1].vPosition;
		}
	}

	if (0 == m_iNowVertexCount)
	{
		for (_uint i = 0; i < m_iNumVertices; i += 2)
		{
			XMStoreFloat3(&pVertices[i].vPosition, vStartPos);
			XMStoreFloat3(&pVertices[i + 1].vPosition, vEndPos);
		}
	}
	
	XMStoreFloat3(&pVertices[m_iNowVertexCount].vPosition, vStartPos);
	XMStoreFloat3(&pVertices[m_iNowVertexCount + 1].vPosition, vEndPos);

	m_iNowVertexCount += 2;

	_uint iEndIndex = m_tTrailDesc.iCatmullRomCount * 2 + m_iNowVertexCount;
	if (iEndIndex < m_iNumVertices)
	{
		m_iCatmullRomIndex[2] = iEndIndex - 2;
		m_iCatmullRomIndex[3] = iEndIndex;

		memcpy(&pVertices[iEndIndex - 2].vPosition, &pVertices[m_iNowVertexCount - 2].vPosition, sizeof(_float3));
		memcpy(&pVertices[iEndIndex - 1].vPosition, &pVertices[m_iNowVertexCount - 1].vPosition, sizeof(_float3));

		XMStoreFloat3(&pVertices[iEndIndex].vPosition, vStartPos);
		XMStoreFloat3(&pVertices[iEndIndex + 1].vPosition, vEndPos);

		for (_uint i = 0; i < m_tTrailDesc.iCatmullRomCount; ++i)
		{
			_uint iIndex = i * 2 + m_iNowVertexCount - 2;
			_float fWeight = _float(i + 1) / (m_tTrailDesc.iCatmullRomCount + 1);

			_vector vPos[4];
			for (_uint j = 0 ; j < 4; ++j)
				vPos[j] = XMLoadFloat3(&pVertices[m_iCatmullRomIndex[j]].vPosition);

			_vector vCatmullRomStartPos = XMVectorCatmullRom(vPos[0], vPos[1], vPos[2], vPos[3], fWeight);
			XMStoreFloat3(&pVertices[iIndex].vPosition, vCatmullRomStartPos);
			 
			for (_uint j = 0; j < 4; ++j)
				vPos[j] = XMLoadFloat3(&pVertices[m_iCatmullRomIndex[j] + 1].vPosition);

			_vector vCatmullRomEndPos = XMVectorCatmullRom(vPos[0], vPos[1], vPos[2], vPos[3], fWeight);
			XMStoreFloat3(&pVertices[iIndex + 1].vPosition, vCatmullRomEndPos);
		}

		m_iNowVertexCount = iEndIndex + 2;

		m_iCatmullRomIndex[0] = m_iCatmullRomIndex[1];
		m_iCatmullRomIndex[1] = m_iCatmullRomIndex[2];
	}

	for (_uint i = 0; i < m_iNowVertexCount; i += 2)
	{
		pVertices[i].vTexture = { (_float)i / (_float)(m_iNowVertexCount - 2), 1.f };
		pVertices[i + 1].vTexture = { (_float)i / (_float)(m_iNowVertexCount - 2), 0.f };
	}

	m_pContext->Unmap(m_pVB, 0);
}

HRESULT CVIBuffer_Trail::Init_Buffers()
{
#pragma region VERTEXBUFFER
	m_iNumVertexBuffers = 1;
	m_iNumVertices = m_tTrailDesc.iMaxVertexCount * 2;
	m_iStride = sizeof(VTXTEX);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXTEX* pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXTEX) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; i += 2)
	{
		pVertices[i].vPosition = m_tTrailDesc.vStartPos;
		pVertices[i + 1].vPosition = m_tTrailDesc.vEndPos;

		pVertices[i].vTexture = { 1.f, 1.f };
		pVertices[i + 1].vTexture = { 1.f, 0.f };
	}

	pVertices[0].vTexture = _float2(0.f, 0.f);
	pVertices[1].vTexture = _float2(0.f, 1.f);

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion
	
#pragma region INDEXBUFFER
	m_iNumPrimitives = m_tTrailDesc.iMaxVertexCount * 2 - 2;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES16);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	FACEINDICES16* pIndices = new FACEINDICES16[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitives);

	for (_uint i = 0; i < m_iNumPrimitives; i += 2)
	{
		pIndices[i]._0 = i + 3;
		pIndices[i]._1 = i + 1;
		pIndices[i]._2 = i;

		pIndices[i + 1]._0 = i + 2;
		pIndices[i + 1]._1 = i + 3;
		pIndices[i + 1]._2 = i;
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Trail::Remake_Buffers(const TRAIL_DESC& TrailDesc)
{
	m_tTrailDesc = TrailDesc;

	if (FAILED(Init_Buffers()))
		return E_FAIL;

	Reset_Points();

	return S_OK;
}

void CVIBuffer_Trail::Reset_Points()
{
	//D3D11_MAPPED_SUBRESOURCE SubResource;
	//
	//m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	//
	//VTXTEX* pVertices = (VTXTEX*)SubResource.pData;
	//
	//for (_uint i = 0; i < m_iNumVertices; i += 2)
	//{
	//	memcpy(&pVertices[i].vPosition, &m_tTrailDesc.vStartPos, sizeof(_float3));
	//	memcpy(&pVertices[i].vPosition, &m_tTrailDesc.vStartPos, sizeof(_float3));
	//
	//}
	//m_pContext->Unmap(m_pVB, 0);

	m_iNowVertexCount = 0;
}

CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CVIBuffer_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Trail::Clone(void* pArg)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CVIBuffer_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	__super::Free();

}
