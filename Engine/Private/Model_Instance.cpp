#include "Model_Instance.h"
#include "Model.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh_Instance.h"
#include "Transform.h"

CModel_Instance::CModel_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CModel_Instance::CModel_Instance(const CModel_Instance& rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_InstanceMeshes(rhs.m_InstanceMeshes)
	, m_Materials(rhs.m_Materials)
{
	for (auto& pInstanceMesh : m_InstanceMeshes)
		Safe_AddRef(pInstanceMesh);

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}
}

HRESULT CModel_Instance::Initialize_Prototype(const string& strModelFilePath, const string& strModelFileName)
{
	if (FAILED(Import_Model(strModelFilePath, strModelFileName)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel_Instance::SetUp_OnShader(CShader* pShader, _uint iMaterialIndex, TextureType eTextureType, const char* pConstantName)
{
	if (iMaterialIndex >= m_iNumMaterials || nullptr == m_Materials[iMaterialIndex].pTexture[eTextureType])
		return E_FAIL;

	return m_Materials[iMaterialIndex].pTexture[eTextureType]->Set_SRV(pShader, pConstantName);
}

HRESULT CModel_Instance::Bind_Buffers(_uint iMeshIdx)
{
	return m_InstanceMeshes[iMeshIdx]->Bind_Buffers();
}

HRESULT CModel_Instance::Render(CShader* pShader, _uint iMeshIndex, _uint iPassIndex)
{
	pShader->Begin(iPassIndex);

	m_InstanceMeshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel_Instance::Ready_Instancing(vector<CTransform*>& InstanceTransforms)
{
	for (auto& pMeshInstance : m_InstanceMeshes)
		pMeshInstance->Update_InstanceBuffer(InstanceTransforms);

	return S_OK;
}

HRESULT CModel_Instance::Import_Model(const string& strFilePath, const string& strFileName)
{
	string strFullPath = strFilePath + strFileName;

	ifstream fin;
	fin.open(strFullPath, ios::binary);

	if (!fin.is_open())
		return E_FAIL;

	_uint iDummy;
	_float4x4 Dummy4x4;

	fin.read((char*)&iDummy, sizeof(_uint));
	fin.read((char*)&Dummy4x4, sizeof(_float4x4));

	if (FAILED(Import_Meshes(fin)))
		return E_FAIL;

	if (FAILED(Import_MaterialInfo(fin, strFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel_Instance::Import_Meshes(ifstream& fin)
{
	fin.read((char*)&m_iNumMeshes, sizeof(_uint));

	m_InstanceMeshes.reserve(m_iNumMeshes);
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh_Instance* pMesh = CMesh_Instance::Create(m_pDevice, m_pContext, fin);
		m_InstanceMeshes.emplace_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel_Instance::Import_MaterialInfo(ifstream& fin, const string& strFilePath)
{
	fin.read((char*)&m_iNumMaterials, sizeof(_uint));

	m_Materials.reserve(m_iNumMaterials);
	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MATERIALDESC MaterialDesc = {};

		for (_uint j = 0; j < TEXTURE_TYPE_MAX; ++j)
		{
			_uint iFileLength = 0;
			fin.read((char*)&iFileLength, sizeof(_uint));
			if (0 == iFileLength)
				continue;

			char szFileName[MAX_PATH] = "";
			fin.read(szFileName, iFileLength);

			string strFullPath = strFilePath + string("Tex/") + string(szFileName);
			wstring wstrFilePath = wstring().assign(strFullPath.begin(), strFullPath.end());

			CTexture* pMatTexture = nullptr;
			auto it = CModel::g_ModelTextures.find(szFileName);
			if (CModel::g_ModelTextures.end() == it)
			{
				pMatTexture = CTexture::Create(m_pDevice, m_pContext, wstrFilePath.c_str());
				if (nullptr == pMatTexture)
					return E_FAIL;

				CModel::g_ModelTextures.emplace(szFileName, pMatTexture);
			}
			else
				pMatTexture = it->second;

			MaterialDesc.pTexture[j] = pMatTexture;
			Safe_AddRef(pMatTexture);
		}
		m_Materials.emplace_back(MaterialDesc);
	}
	return S_OK;
}

CModel_Instance* CModel_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath, const string& strModelFileName)
{
	CModel_Instance* pInstance = new CModel_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strModelFilePath, strModelFileName)))
	{
		MSG_BOX(TEXT("Failed To Created : CModel"));
		assert(false);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel_Instance::Clone(void* pArg)
{
	return new CModel_Instance(*this);
}

void CModel_Instance::Free()
{
	__super::Free();

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < TEXTURE_TYPE_MAX; ++i)
			Safe_Release(Material.pTexture[i]);
	}
	m_Materials.clear();

	for (auto& pMeshInstance: m_InstanceMeshes)
		Safe_Release(pMeshInstance);

	m_InstanceMeshes.clear();

}
