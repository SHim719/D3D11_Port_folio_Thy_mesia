#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CGameEffect abstract : public CGameObject
{
public:
	enum EFFECTTYPE { PARTICLE, MESH, TRAIL, EFFECT_END };
	enum TRANSFORMTYPE { SPAWN_ONLY, FOLLOW_PARENT, TF_END}; // 월드 위치를 어떻게 가져갈것인가.

public:
	typedef struct tagEffectSpawnDesc
	{
		CTransform*		pParentTransform = nullptr;
		CModel*			pParentModel = nullptr;
	}EFFECTSPAWNDESC;

protected:
	CGameEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameEffect(const CGameEffect& rhs);
	virtual ~CGameEffect() = default;

public:
	virtual HRESULT Save_EffectData(ofstream& fout);
	virtual HRESULT Load_EffectData(ifstream& fin);

	virtual void Restart_Effect(EFFECTSPAWNDESC* pDesc = nullptr);
	virtual void OnEnd_Effect();

	_bool Is_Complete() const {
		return m_bComplete;
	}

protected:
	void Update_World();
	void Update_TextureFlag();

	void Spawn_Effect();

protected:
	EFFECTTYPE		m_eEffectType = { EFFECT_END };
	TRANSFORMTYPE	m_eTransformType = { SPAWN_ONLY };
	_char			m_szEffectName[MAX_PATH] = "";

	_float			m_fPlayTime = { 0.f };
	_bool			m_bComplete = { false };

	_int			m_iBaseTextureIdx = { 0 };
	_int			m_iMaskTextureIdx = { -1 };
	_int			m_iNoiseTextureIdx = { -1 };
	_int			m_iEmissiveTextureIdx = { -1 };
	_int			m_iPassIdx = { 0 };

	_int			m_iRenderGroup = CRenderer::RENDER_EFFECT_BLEND;

	_float4			m_vTextureFlag = { 0.f, 0.f, 0.f, 0.f };

	_bool			m_bLoop = { true };
	_float			m_fTimeAcc = { 0.f };

	_float4			m_vClipColor = { 0.f, 0.f, 0.f, 0.f };

	_bool			m_bGlow = { false };
	_float4			m_vGlowColor = { 1.f, 1.f, 1.f, 1.f };
	_float			m_fGlowIntensity = { 1.f };

	_bool			m_bBloom = { false };

	CTransform*		m_pParentTransform = { nullptr };
	CBone*			m_pParentBone = { nullptr };

	_char			m_szBoneName[MAX_PATH] = {};
	_float3			m_vWorldOffset = { 0.f, 0.f, 0.f };
	_bool			m_bFollowOnlyPosition = { false };

	_bool			m_bSyncKeyFrame = { false };
	_int			m_iSyncKeyFrame = { 0 };

protected:
	CTexture*		m_pBaseTexture = { nullptr };
	CTexture*		m_pMaskTexture = { nullptr };
	CTexture*		m_pNoiseTexture = { nullptr };
	CTexture*		m_pEmissiveTexture = { nullptr };

	CShader*		m_pShader = { nullptr };

protected:
	virtual HRESULT Bind_GlobalVariables();
	virtual HRESULT Bind_ShaderResources();

public:
	virtual CGameObject* Clone(void* pArg)	= 0;
	void Free() override;

	friend class CAnim_Tool;
};

END

