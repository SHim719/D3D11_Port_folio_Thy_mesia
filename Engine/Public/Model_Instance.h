#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel_Instance final : public CComponent
{
private:
	CModel_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel_Instance(const CModel_Instance& rhs);
	virtual ~CModel_Instance() = default;

public:
	HRESULT Initialize_Prototype(const string& strModelFilePath, const string& strModelFileName);

	HRESULT SetUp_OnShader(class CShader* pShader, _uint iMaterialIndex, TextureType eTextureType, const char* pConstantName);
	HRESULT Bind_Buffers(_uint iMeshIdx);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0); 
	
private:
	_uint									m_iNumMeshes = { 0 };
	vector<class CMesh_Instance*>			m_InstanceMeshes;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	HRESULT Ready_Instancing(vector<class CTransform*>& InstanceTransforms);

private:
	_uint													m_iNumMaterials = { 0 };
	vector<MATERIALDESC>									m_Materials;

private:
	HRESULT Import_Model(const string& strFilePath, const string& strFileName);
	HRESULT Import_Meshes(ifstream& fin);
	HRESULT Import_MaterialInfo(ifstream& fin, const string& strFilePath);

public:
	static CModel_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath, const string& strModelFileName);
	 CComponent* Clone(void* pArg = nullptr)	override;
	 void Free() override;
};

END

