#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_SKELETALMESH, TYPE_ANIM, TYPE_END };
	typedef vector<class CBone*>	BONES;
	typedef vector<class CMeshContainer*>	MESHES;

public:
	typedef struct tagLoadModelDesc
	{
		TYPE eType;
		string strModelFilePath;
		string strModelFileName;
		_float4x4 PivotMatrix;
	}LOADMODELDESC;
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	class CBone* Get_Bone(const char* pNodeName);

	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_uint Get_MaterialIndex(_uint iMeshIndex);

	_matrix Get_PivotMatrix() {
		return XMLoadFloat4x4(&m_PivotMatrix);
	}


public:
	HRESULT Initialize_Prototype(void* pArg);
	HRESULT Initialize(void* pArg)	override;

public:
	HRESULT SetUp_OnShader(class CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* pConstantName);
	HRESULT Play_Animation(_float fTimeDelta);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0);

private:
	const aiScene*				m_pAIScene = nullptr;
	Assimp::Importer			m_Importer;

	_float4x4					m_PivotMatrix;
	TYPE						m_eModelType = TYPE_END;

private:
	_uint									m_iNumMeshes = 0;
	vector<class CMeshContainer*>			m_Meshes;
	
private:
	_uint									m_iNumMaterials = 0;
	vector<MATERIALDESC>					m_Materials;
	
private:
	vector<class CBone*>					m_Bones;

private:
	_uint									m_iCurrentAnimIndex = 0;
	_uint									m_iNumAnimations = 0;
	vector<class CAnimation*>				m_Animations;

public:
	_uint	Get_NumAnimations() const { return m_iNumAnimations; }
	_uint	Get_CurrentAnimIndex() const { return m_iCurrentAnimIndex; }
	void	Set_AnimIndex(_uint iAnimIndex) { m_iCurrentAnimIndex = iAnimIndex; }


private:
	class CVTF*		m_pVTF = nullptr;

public:
	void	Update_VTF(_uint iMeshIndex);
	HRESULT Bind_VTF(CShader* pShader, const char* pConstantName, _uint iMeshIndex);

private:
	HRESULT Ready_MeshContainers(_fmatrix PivotMatrix);
	HRESULT Ready_Materials(const char* pModelFilePath);
	HRESULT Ready_Bones(aiNode* pNode, class CBone* pParent, _uint iDepth);
	HRESULT Ready_Animations();
	
public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END