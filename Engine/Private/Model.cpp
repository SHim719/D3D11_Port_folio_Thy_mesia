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

	// aiProcess_PreTransformVertices : 모델을 구성하는 메시 중, 이 메시의 이름과 뼈의 이름이 같은 상황이라면 이 뼈의 행렬을 메시의 정점에 다 곱해서 로드한다. 
	// 모든 애니메이션 정보는 폐기된다. 
	if (TYPE_NONANIM == m_eModelType)
		iFlag |= aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;
	else
		iFlag |= aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;

	/* 파일의 정보를 읽어서 aiScene안에 보관한다.  */
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


	/* 뼈대 정볼르 로드하낟. */
	/* 이 모델 전체의 뼈의 정보를 로드한다. */
	/* Bone : 뼈의 상태를 가진다.(offSetMatrix, Transformation, CombinedTransformation */
	Ready_Bones(m_pAIScene->mRootNode, nullptr, 0);

	/* 뎁스로 정렬한다. */
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

	/* 현재 재생하고자하는 애니메이션이 제어해야할 뼈들의 지역행렬을 갱신해낸다. */
	m_Animations[m_iCurrentAnimIndex]->Play_Animation(fTimeDelta);

	/* 지역행렬을 순차적으로(부모에서 자식으로) 누적하여 m_CombinedTransformation를 만든다.  */
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
	/* 메시의 갯수를 얻어온다. */
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

		/* AI_TEXTURE_TYPE_MAX:디퓨즈or앰비언트or노말or이미시브 등등등 */
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString		strPath;

			/* 해당 재질을 표현하기위한 텍스쳐의 경로를 strPath에 받아온다. */
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
	/* pParent? : 부모 노드 주소. CombinedTransformation으로 그린다.
	CombinedTransformation놈을 만들려면 나의 Transformation * 부모의CombinedTranfsormation로 만든다. */
	/* 그래서 부모가 필요해. */
	/* iDepth? : Ready_Bones함수를 재귀형태로 부르고ㅓ있기ㄸ매ㅜㄴ에 한쪽(깊이)으로 생성해나가기 때문에. */
	/* 이후 애님에ㅣ션 재생할때 뼈의 CombinedTransformation를 만든다. */
	/* CombinedTransformation만들려면 부모의 상태가 모두 갱신되어있어야돼. 왜 부모의 컴바인드 이용하니까.ㄴ ==
	 부모부터 자식으로 순차적으로 CombinedTransformation를 만들어야한다라는 걸 의미.  */
	 /* m_Bones컨테이너는 최상위 부모가 가장 앞에 있어야한다. 이놈의 1차 자식들이 두번째에 쫙. 삼차짜식들이 그다음쫘악. */
	 /* 각 노드마다 깊이값(몇차자식이냐? ) 을 저장해두고 나중에 정렬한다. */
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
