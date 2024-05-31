#include "..\Public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "Bone.h"
#include "Animation.h"
#include "Shader.h"


map<const string, class CTexture*>	CModel::g_ModelTextures;


CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
	XMStoreFloat4x4(&m_PivotMatrix, XMMatrixIdentity());
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Meshes(rhs.m_Meshes)
	, m_Materials(rhs.m_Materials)
	, m_eModelType(rhs.m_eModelType)
	, m_BoneIndices(rhs.m_BoneIndices)
	, m_iNumBones(rhs.m_iNumBones)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_PivotMatrix(rhs.m_PivotMatrix)
{
	for (auto& pMeshContainer : m_Meshes)
		Safe_AddRef(pMeshContainer);

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}
}

CBone* CModel::Get_Bone(const char* pNodeName)
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pNode)
		{
			return !strcmp(pNodeName, pNode->Get_Name());
		});

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;
}

_uint CModel::Find_BoneIndex(const char* pBoneName)
{
	_uint iBoneIdx = 0;
	for (CBone* pBone : m_Bones)
	{
		if (!strcmp(pBoneName, pBone->Get_Name()))
			break;

		++iBoneIdx;
	}

	return iBoneIdx;
}

_uint CModel::Get_MaterialIndex(_uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Get_MaterialIndex();
}

HRESULT CModel::Initialize_Prototype(const string& strModelFilePath, const string& strModelFileName)
{
	if (FAILED(Import_Model(strModelFilePath, strModelFileName)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg, CModel* pModel)
{
	const vector<CAnimation*>& Animations = pModel->Get_Animations();
	m_Animations.reserve(Animations.size());
	for (size_t i = 0; i < Animations.size(); ++i)
	{
		CAnimation* pAnimation = Animations[i]->Clone();
		m_Animations.emplace_back(pAnimation);
	}

	const BONES& Bones = pModel->Get_Bones();
	m_Bones.reserve(Bones.size());
	for (size_t i = 0; i < Bones.size(); ++i)
	{
		CBone* pBone = Bones[i]->Clone();
		m_Bones.emplace_back(pBone);
	}

	return S_OK;
}

HRESULT CModel::SetUp_BoneMatrices(CShader* pShader)
{
	if (0 == m_iNumAnimations || !m_bIsPlaying)
		return S_OK;

	_float4x4 BoneMatrices[256];

	// NO_VTF
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		XMStoreFloat4x4(&BoneMatrices[i], XMMatrixTranspose(m_Bones[m_BoneIndices[i]]->Get_OffSetMatrix()
			* m_Bones[m_BoneIndices[i]]->Get_CombinedTransformation()
			* XMLoadFloat4x4(&m_PivotMatrix)));
	}

	return pShader->Set_RawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * 256);
}

HRESULT CModel::SetUp_OnShader(CShader* pShader, _uint iMaterialIndex, TextureType eTextureType, const char* pConstantName)
{
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex].pTexture[eTextureType]->Set_SRV(pShader, pConstantName);
}

HRESULT CModel::Play_Animation(_float fTimeDelta)
{
	if (!m_bIsPlaying || m_iCurrentAnimIndex >= m_iNumAnimations)
		return E_FAIL;

	if (m_bBlending)
	{
		m_Animations[m_iCurrentAnimIndex]->Play_Animation_Blend(fTimeDelta, m_Bones);
		m_fBlendingTime -= fTimeDelta;
		if (m_fBlendingTime <= 0.f)
		{
			m_fBlendingTime = 0.f;
			m_bBlending = false;
		}
	}
	
	else
	{
		m_Animations[m_iCurrentAnimIndex]->Play_Animation(fTimeDelta, m_Bones);
	}
		

	for (auto& pBone : m_Bones)
	{
		pBone->Set_CombinedTransformation(m_Bones);
	}

	return S_OK;
}


HRESULT CModel::Render(CShader* pShader, _uint iMeshIndex, _uint iPassIndex)
{
	pShader->Begin(iPassIndex);

	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

void CModel::Set_PivotMatrix(_fmatrix PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);
}

void CModel::Change_Animation(_uint iAnimIdx, _float fBlendingTime)
{
	if (iAnimIdx < (_uint)m_Animations.size())
		m_Animations[m_iCurrentAnimIndex]->Reset();

	m_iCurrentAnimIndex = iAnimIdx;

	if (fBlendingTime > 0.f)
	{
		m_bBlending = true;
		m_fBlendingTime = fBlendingTime;
		m_Animations[m_iCurrentAnimIndex]->Set_BlendingTime(fBlendingTime);
	}
	
}

HRESULT CModel::Import_Model(const string& strFilePath, const string& strFileName)
{
	string strFullPath = strFilePath + strFileName;

	ifstream fin;
	fin.open(strFullPath, ios::binary);
	if (!fin.is_open())
		return E_FAIL;

	fin.read((char*)&m_eModelType, sizeof(TYPE));
	fin.read((char*)&m_PivotMatrix, sizeof(_float4x4));

	if (FAILED(Import_Meshes(fin)))
		return E_FAIL;

	if (FAILED(Import_MaterialInfo(fin, strFilePath)))
		return E_FAIL;

	if (TYPE_ANIM == m_eModelType)
	{
		if (FAILED(Import_Bones(fin)))
			return E_FAIL;

		if (FAILED(Import_Animations(fin)))
			return E_FAIL;
	}

	fin.close();

	return S_OK;
}

HRESULT CModel::Import_Meshes(ifstream& fin)
{
	fin.read((char*)&m_iNumMeshes, sizeof(_uint));

	m_Meshes.reserve(m_iNumMeshes);
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMeshContainer* pMesh = CMeshContainer::Create(m_pDevice, m_pContext, fin, m_eModelType);
		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Import_MaterialInfo(ifstream& fin, const string& strFilePath)
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
			auto it = g_ModelTextures.find(szFileName);
			if (g_ModelTextures.end() == it)
			{
				pMatTexture = CTexture::Create(m_pDevice, m_pContext, wstrFilePath.c_str());
				if (nullptr == pMatTexture)
					return E_FAIL;

				g_ModelTextures.insert({ szFileName, pMatTexture });
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

HRESULT CModel::Import_Bones(ifstream& fin)
{
	_uint iOriginBoneSize = 0;
	fin.read((char*)&iOriginBoneSize, sizeof(_uint));

	m_Bones.reserve(iOriginBoneSize);
	for (size_t i = 0; i < iOriginBoneSize; ++i)
	{
		CBone* pBone = CBone::Create(fin, this);
		if (nullptr == pBone)
			return E_FAIL;
		m_Bones.push_back(pBone);
	}

	m_iNumBones = 0;
	fin.read((char*)&m_iNumBones, sizeof(_uint));

	m_BoneIndices.reserve(m_iNumBones);
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		_uint iBoneIdx = 0;
		fin.read((char*)&iBoneIdx, sizeof(_uint));
		m_BoneIndices.push_back(iBoneIdx);
	}

	return S_OK;
}

HRESULT CModel::Import_Animations(ifstream& fin)
{
	if (TYPE_NONANIM == m_eModelType)
		return S_OK;

	fin.read((char*)&m_iNumAnimations, sizeof(_uint));

	m_Animations.reserve(m_iNumAnimations);
	for (size_t i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation* pAnim = CAnimation::Create(fin);
		m_Animations.push_back(pAnim);
	}
	return S_OK;
}


CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath, const string& strModelFileName)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strModelFilePath, strModelFileName)))
	{
		MSG_BOX(TEXT("Failed To Created : CModel"));
		assert(false);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Release_Textures()
{
	for (auto& Pairs : g_ModelTextures)
	{
		Safe_Release(Pairs.second);
	}

	g_ModelTextures.clear();
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg, this)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CModel"));
		assert(false);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < TEXTURE_TYPE_MAX; ++i)
			Safe_Release(Material.pTexture[i]);
	}
	m_Materials.clear();

	for (auto& pMeshContainer : m_Meshes)
		Safe_Release(pMeshContainer);

	m_Meshes.clear();
}


//void CModel::Update_VTF(_uint iMeshIndex)
//{
//	if (TYPE_ANIM != m_eModelType)
//		return;
//
//	_float4x4		BoneMatrices[1024];
//
//	m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, m_Bones, XMLoadFloat4x4(&m_PivotMatrix));
//
//	m_pVTF->Update_VTF(BoneMatrices, iMeshIndex);
//
//}
//
//HRESULT CModel::Bind_VTF(CShader* pShader, const char* pConstantName, _uint iMeshIndex)
//{
//	return m_pVTF->Bind_ShaderResourceView(pShader, pConstantName, iMeshIndex);
//}
