#include "..\Public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "Bone.h"
#include "Animation.h"
#include "Shader.h"
#include "VTF.h"


CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_pAIScene(rhs.m_pAIScene)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Meshes(rhs.m_Meshes)
	, m_Materials(rhs.m_Materials)
	, m_eModelType(rhs.m_eModelType)
	/*, m_Bones(rhs.m_Bones)*/
	, m_Animations(rhs.m_Animations)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumAnimations(rhs.m_iNumAnimations)

{
	for (auto& pMeshContainer : m_Meshes)
		Safe_AddRef(pMeshContainer);


	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}

	for (auto& pAnimation : m_Animations)
		Safe_AddRef(pAnimation);

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

_uint CModel::Get_MaterialIndex(_uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Get_MaterialIndex();
}

HRESULT CModel::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	LOADMODELDESC* modelDesc = (LOADMODELDESC*)pArg;

	m_PivotMatrix = modelDesc->PivotMatrix;

	char		szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, modelDesc->pModelFilePath);
	strcat_s(szFullPath, modelDesc->pModelFileName);

	_uint		iFlag = 0;

	m_eModelType = modelDesc->eType;

	// aiProcess_PreTransformVertices : ���� �����ϴ� �޽� ��, �� �޽��� �̸��� ���� �̸��� ���� ��Ȳ�̶�� �� ���� ����� �޽��� ������ �� ���ؼ� �ε��Ѵ�. 
	// ��� �ִϸ��̼� ������ ���ȴ�. 
	if (TYPE_NONANIM == m_eModelType)
		iFlag |= aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;
	else
		iFlag |= aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;

	/* ������ ������ �о aiScene�ȿ� �����Ѵ�.  */
	m_pAIScene = m_Importer.ReadFile(szFullPath, iFlag);

	if (nullptr == m_pAIScene)
		return E_FAIL;

	if (FAILED(Ready_MeshContainers(XMLoadFloat4x4(&(modelDesc->PivotMatrix)))))
		return E_FAIL;


	if (FAILED(Ready_Materials(modelDesc->pModelFilePath)))
		return E_FAIL;

	if (TYPE_ANIM == modelDesc->eType)
	{
		if (FAILED(Ready_Animations()))
			return E_FAIL;
	}


	/* ���� ������ �ε��ϳ�. */
	/* �� �� ��ü�� ���� ������ �ε��Ѵ�. */
	/* Bone : ���� ���¸� ������.(offSetMatrix, Transformation, CombinedTransformation */
	Ready_Bones(m_pAIScene->mRootNode, nullptr, 0);

	/* ������ �����Ѵ�. */
	/*sort(m_Bones.begin(), m_Bones.end(), [](CBone* pSour, CBone* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});*/

	if (TYPE_NONANIM != m_eModelType)
	{
		_uint		iNumMeshes = 0;

		vector<CMeshContainer*>		MeshContainers;

		for (auto& pPrototype : m_Meshes)
		{
			CMeshContainer* pMeshContainer = (CMeshContainer*)pPrototype->Clone();
			if (nullptr == pMeshContainer)
				return E_FAIL;

			MeshContainers.push_back(pMeshContainer);

			Safe_Release(pPrototype);
		}

		m_Meshes.clear();

		m_Meshes = MeshContainers;

		for (auto& pMeshContainer : m_Meshes)
		{
			if (nullptr != pMeshContainer)
				pMeshContainer->SetUp_Bones(this, m_pAIScene->mMeshes[iNumMeshes++]);
		}
	}
	if (TYPE_ANIM == m_eModelType)
	{
		m_pVTF = CVTF::Create(m_pDevice, m_pContext, this);
		if (nullptr == m_pVTF)
			return E_FAIL;

		vector<CAnimation*>		Animations;

		for (auto& pPrototype : m_Animations)
		{
			CAnimation* pAnimation = pPrototype->Clone(this);
			if (nullptr == pAnimation)
				return E_FAIL;

			Animations.push_back(pAnimation);

			Safe_Release(pPrototype);
		}

		m_Animations.clear();

		m_Animations = Animations;
	}
	return S_OK;
}

HRESULT CModel::SetUp_OnShader(CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* pConstantName)
{
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex].pTexture[eTextureType]->Set_SRV(pShader, pConstantName);
}

HRESULT CModel::Play_Animation(_float fTimeDelta)
{
	if (m_iCurrentAnimIndex >= m_iNumAnimations)
		return E_FAIL;

	/* ���� ����ϰ����ϴ� �ִϸ��̼��� �����ؾ��� ������ ��������� �����س���. */
	m_Animations[m_iCurrentAnimIndex]->Play_Animation(fTimeDelta);

	/* ��������� ����������(�θ𿡼� �ڽ�����) �����Ͽ� m_CombinedTransformation�� �����.  */
	for (auto& pBone : m_Bones)
	{
		pBone->Set_CombinedTransformation();
	}

	return S_OK;
}

HRESULT CModel::Render(CShader* pShader, _uint iMeshIndex, _uint iPassIndex)
{
	pShader->Begin(iPassIndex);

	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

void CModel::Update_VTF(_uint iMeshIndex)
{
	if (TYPE_ANIM != m_eModelType)
		return;

	_float4x4		BoneMatrices[1024];

	m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, m_Bones, XMLoadFloat4x4(&m_PivotMatrix));

	m_pVTF->Update_VTF(BoneMatrices, iMeshIndex);
	
}

HRESULT CModel::Bind_VTF(CShader* pShader, const char* pConstantName, _uint iMeshIndex)
{
	return m_pVTF->Bind_ShaderResourceView(pShader, pConstantName, iMeshIndex);
}

HRESULT CModel::Ready_MeshContainers(_fmatrix PivotMatrix)
{
	/* �޽��� ������ ���´�. */
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (_uint i = 0 ; i < m_iNumMeshes; ++i)
	{
		CMeshContainer* pMeshContainer = CMeshContainer::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], this, PivotMatrix);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_Meshes.push_back(pMeshContainer);
	}
	
	return S_OK;
}

HRESULT CModel::Ready_Materials(const char* pModelFilePath)
{
	if (nullptr == m_pAIScene)
		return E_FAIL;

	
	m_iNumMaterials = m_pAIScene->mNumMaterials;
	m_Materials.reserve(m_iNumMaterials);

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MATERIALDESC		MaterialDesc;
		ZeroMemory(&MaterialDesc, sizeof(MATERIALDESC));

		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		/* AI_TEXTURE_TYPE_MAX:��ǻ��or�ں��Ʈor�븻or�̹̽ú� ���� */
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString		strPath;

			/* �ش� ������ ǥ���ϱ����� �ؽ����� ��θ� strPath�� �޾ƿ´�. */
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strPath)))
				continue;

			char			szFullPath[MAX_PATH] = "";
			char			szFileName[MAX_PATH] = "";
			char			szExt[MAX_PATH] = "";

			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szFullPath, pModelFilePath);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar			szWideFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (int)strlen(szFullPath), szWideFullPath, MAX_PATH);

			MaterialDesc.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szWideFullPath);
			if (nullptr == MaterialDesc.pTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(MaterialDesc);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode* pNode, CBone* pParent, _uint iDepth)
{
	/* pParent? : �θ� ��� �ּ�. CombinedTransformation���� �׸���.
	CombinedTransformation���� ������� ���� Transformation * �θ���CombinedTranfsormation�� �����. */
	/* �׷��� �θ� �ʿ���. */
	/* iDepth? : Ready_Bones�Լ��� ������·� �θ�����ֱ⤨�Ť̤��� ����(����)���� �����س����� ������. */
	/* ���� �ִԿ��Ӽ� ����Ҷ� ���� CombinedTransformation�� �����. */
	/* CombinedTransformation������� �θ��� ���°� ��� ���ŵǾ��־�ߵ�. �� �θ��� �Ĺ��ε� �̿��ϴϱ�.�� ==
	 �θ���� �ڽ����� ���������� CombinedTransformation�� �������Ѵٶ�� �� �ǹ�.  */
	 /* m_Bones�����̳ʴ� �ֻ��� �θ� ���� �տ� �־���Ѵ�. �̳��� 1�� �ڽĵ��� �ι�°�� ��. ����¥�ĵ��� �״����Ҿ�. */
	 /* �� ��帶�� ���̰�(�����ڽ��̳�? ) �� �����صΰ� ���߿� �����Ѵ�. */
	CBone* pBone = CBone::Create(pNode, pParent, iDepth++);

	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		Ready_Bones(pNode->mChildren[i], pBone, iDepth);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (_uint i = 0; i < m_pAIScene->mNumAnimations; ++i)
	{
		aiAnimation* pAIAnimation = m_pAIScene->mAnimations[i];

		CAnimation* pAnimation = CAnimation::Create(pAIAnimation);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}
	return S_OK;
}


CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const char* pModelFilePath, const char* pModelFileName, const vector<_uint>& LoadIndices,
	_fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, pModelFileName, LoadIndices, PivotMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(Material.pTexture[i]);
	}
	m_Materials.clear();

	for (auto& pMeshContainer : m_Meshes)
		Safe_Release(pMeshContainer);

	m_Meshes.clear();

	m_Importer.FreeScene();
}
