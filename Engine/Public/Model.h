#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
	typedef vector<class CBone*>			BONES;
	typedef vector<class CMeshContainer*>	MESHES;
	typedef vector<class CAnimation*>		ANIMATIONS;
	typedef unordered_map<string, class CKeyFrameEvent*> KEYFRAMEEVENTS;

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	class CBone* Get_Bone(const char* pNodeName);

	_uint Find_BoneIndex(const char* pBoneName);

	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_matrix Get_PivotMatrix() {
		return XMLoadFloat4x4(&m_PivotMatrix);
	}

public:
	HRESULT Initialize_Prototype(const string& strModelFilePath, const string& strModelFileName, const string& strKeyFrameFilePath);
	HRESULT Initialize(const BONES& Bones, const ANIMATIONS& Anims, const KEYFRAMEEVENTS& Events);

public:
	HRESULT SetUp_BoneMatrices(class CShader* pShader);
	HRESULT SetUp_OnShader(class CShader* pShader, _uint iMaterialIndex, TextureType eTextureType, const char* pConstantName);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0);

	void Set_PivotMatrix(_fmatrix PivotMatrix);

	_bool Picking(_fmatrix InvWorldMat, _fvector vRayStartPos, _fvector vRayDir, OUT _float4& vPickedPos, OUT _float& fDist);
private:
	_float4x4					m_PivotMatrix;
	TYPE						m_eModelType = TYPE_END;

	_uint									m_iNumMeshes = { 0 };
	vector<class CMeshContainer*>			m_Meshes;

private:
	static unordered_map<string, class CTexture*>			g_ModelTextures;
	_uint													m_iNumMaterials = { 0 };
	vector<MATERIALDESC>									m_Materials;

private:
	vector<class CBone*>					m_Bones;
	vector<_uint>							m_BoneIndices;
	_uint									m_iNumBones = { 0 };
	_uint									m_iRootBoneIdx = { 0 };
	_float4									m_vPrevRootPos = {};
	_float4									m_vDeltaRootPos = {};
public:
	const BONES& Get_Bones() { 
		return m_Bones; }

	_vector Get_DeltaRootPos() {
		return Organize_RootPos(XMLoadFloat4(&m_vDeltaRootPos));
	}

	_vector Organize_RootPos(_fvector OriginRootPos) const {
		return XMVectorSet(OriginRootPos.m128_f32[0], OriginRootPos.m128_f32[2], -OriginRootPos.m128_f32[1], 1.f);
	}

	void Reset_PrevRootPos();
	_vector Get_NowRootQuat() const;

private:
	void Calc_DeltaRootPos();

private:
	_uint											m_iCurrentAnimIndex = { 0 };
	_uint											m_iNumAnimations = { 0 };
	vector<class CAnimation*>						m_Animations;

	_bool											m_bPreview = { false };
	_bool											m_bIsPlaying = { false };
	_bool											m_bBlending = { false };
	_bool											m_bComplete = { false };
	_bool											m_bLoop = { true };

	unordered_map<string, class CKeyFrameEvent*>	m_AllKeyFrameEvents;
public:
	const vector<CAnimation*>& Get_Animations() const { return m_Animations; }

	_uint	Get_NumAnimations() const { return m_iNumAnimations; }
	_uint	Get_CurrentAnimIndex() const { return m_iCurrentAnimIndex; }

	void		Play_Animation(_float fTimeDelta);
	void		Change_Animation(_uint iAnimIdx, _float fBlendingTime = 0.1f, _bool bLoop = true);
	void		Change_AnimationWithStartFrame(_uint iAnimIdx, _uint iStartKeyFrame, _float fBlendingTime = 0.1f, _bool bLoop = true);
	_bool		Is_Playing() const { return m_bIsPlaying; }
	void		Set_AnimPlay() { m_bIsPlaying = true; }
	void		Set_AnimPause() { m_bIsPlaying = false; }
	void		Set_Preview(_bool b) { m_bPreview = b; }
	_bool		Is_AnimComplete() const { return m_bComplete; }
	void		Set_NowAnimKeyFrame(_uint iKeyFrame, _bool bBlend);

	HRESULT		Bind_Func(const string& strEventName, function<void()> pFunc);

private:
	HRESULT Import_Model(const string& strFilePath, const string& strFileName, const string& strKeyFramefolderPath);
	HRESULT Import_Meshes(ifstream& fin);
	HRESULT Import_MaterialInfo(ifstream& fin, const string& strFilePath);
	HRESULT Import_Bones(ifstream& fin);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
	HRESULT Import_Animations(ifstream& fin);
	HRESULT Import_KeyFrameEvents(const string& strKeyFramefolderPath);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath, const string& strModelFileName,
		const string& strKeyFrameFilePath = "");
	static void Release_Textures();
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;

	friend class CModel_Instance;
};

END




//private:
//	class CVTF* m_pVTF = nullptr;
//	void	Update_VTF(_uint iMeshIndex);
//HRESULT Bind_VTF(CShader* pShader, const char* pConstantName, _uint iMeshIndex);