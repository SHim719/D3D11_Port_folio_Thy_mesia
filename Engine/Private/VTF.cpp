#include "VTF.h"

#include "Shader.h"
#include "Model.h"

CVTF::CVTF(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice) , m_pContext(pContext)
{
}

HRESULT CVTF::Initialize(void* pArg)
{
	CModel* pModel = (CModel*)pArg;
	m_iNumMeshes = pModel->Get_NumMeshes();

	D3D11_TEXTURE2D_DESC VTFTexDesc = {};

	VTFTexDesc.Width = 64;
	VTFTexDesc.Height = 64;
	VTFTexDesc.MipLevels = 1;
	VTFTexDesc.ArraySize = 1;
	VTFTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	VTFTexDesc.SampleDesc.Quality = 0;
	VTFTexDesc.SampleDesc.Count = 1;

	VTFTexDesc.Usage = D3D11_USAGE_DYNAMIC;
	VTFTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	VTFTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VTFTexDesc.MiscFlags = 0;

	m_Textures = new ID3D11Texture2D * [m_iNumMeshes];
	m_SRVs = new ID3D11ShaderResourceView * [m_iNumMeshes];

	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		if (FAILED(m_pDevice->CreateTexture2D(&VTFTexDesc, nullptr, &m_Textures[i])))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateShaderResourceView(m_Textures[i], nullptr, &m_SRVs[i])))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CVTF::Bind_ShaderResourceView(CShader* pShader, const char* pConstantName, _uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	return pShader->Set_ShaderResourceView(pConstantName, m_SRVs[iMeshIndex]);
}

void CVTF::Update_VTF(_float4x4* BoneMatrices, _uint iMeshIdx)
{
	if (iMeshIdx >= m_iNumMeshes)
		return;

	//// Write Texture
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource;

	m_pContext->Map(m_Textures[iMeshIdx], 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

	memcpy(MappedSubResource.pData, BoneMatrices, sizeof(_float4x4) * 1024);

	m_pContext->Unmap(m_Textures[iMeshIdx], 0);
	
}

CVTF* CVTF::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CVTF* pInstance = new CVTF(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX(L"Failed to Created : CVTF");
	}

	return pInstance;
}


void CVTF::Free()
{
	__super::Free();

	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		Safe_Release(m_SRVs[i]);
		Safe_Release(m_Textures[i]);
	}
	Safe_Delete_Array(m_SRVs);
	Safe_Delete_Array(m_Textures);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
