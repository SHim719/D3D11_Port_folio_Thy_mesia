#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CToolEffect_Particle final : public CGameObject
{
private:
	CToolEffect_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolEffect_Particle(const CToolEffect_Particle& rhs);
	virtual ~CToolEffect_Particle() = default;

private:
	HRESULT Initialize_Prototype()			override;
	HRESULT Initialize(void* pArg)			override;

	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;

	HRESULT Render()					override;

	void Update_TextureFlag();
	void Update_Positions(size_t iIdx, _float fTimeDelta);
	void Update_Scale(size_t iIdx);
	void Update_Velocity(size_t iIdx);
	void Update_Color(size_t iIdx);

public:
	void Restart_Particle();
	void Remake_Particle();
	void Resize_Particles(_uint iNumParticles);

private:
	vector<VTXPARTICLE>			m_InitParticleDatas;
	vector<VTXPARTICLE>			m_NowParticleDatas;
	
	vector<PARTICLE_DESC>		m_InitParticleDescs;
	vector<PARTICLE_DESC>		m_NowParticleDescs;

	_int						m_iNumParticles = { 0 };
	_int						m_iBaseTextureIdx = { 0 };
	_int						m_iMaskTextureIdx = { 0 };
	_int						m_iNoiseTextureIdx = { 0 };
	_int						m_iEmissiveTextureIdx = { -1 };
	_int						m_iPassIdx = { 0 };

	_float4						m_vTextureFlag = { 0.f, 0.f, 0.f, 0.f };

	_bool						m_bLoop = { true };
	_bool						m_bBillBoard = { false };
	_bool						m_bAlignedByVelocity = { false };

	_bool						m_bSprite = { false };
	_int						m_iCol = { 0 };
	_int						m_iRow = { 0 };
	_int						m_iNowSpriteIdx = { 0 };

private:
	_float3						m_vStartPosMin = { 0.f, 0.f, 0.f };
	_float3						m_vStartPosMax = { 0.f, 0.f, 0.f };

	_float2						m_vStartSizeMin = { 1.f, 1.f };
	_float2						m_vStartSizeMax = { 1.f, 1.f };

	_float3						m_vSpeedDirMin = { 0.f, 0.f, 0.f };
	_float3						m_vSpeedDirMax = { 0.f, 0.f, 0.f };

	_float						m_fStartSpeedMin = { 0.f };
	_float						m_fStartSpeedMax = { 0.f };

	_float						m_fGravityScale = { 0.f };

	_float4						m_vStartColorMin = { 1.f, 1.f, 1.f, 1.f };
	_float4						m_vStartColorMax = { 1.f, 1.f, 1.f, 1.f };
	_float4						m_vColorMul = { 1.f, 1.f, 1.f, 1.f };
	_float4						m_vColorOffset = { 0.f, 0.f, 0.f, 0.f };
	_float4						m_vClipColor = { 0.f, 0.f, 0.f, 0.f };

	_float						m_fLifeTimeMin = { 1.f };
	_float						m_fLifeTimeMax = { 1.f };

	_bool						m_bSpeedLerp = { false };
	_float						m_fSpeedEnd = { 0.f };

	_bool						m_bSizeLerp = { false };
	_float2						m_vSizeEnd = { 0.f, 0.f };

	_bool						m_bColorLerp = { false };
	_float4						m_vColorEnd = { 0.f, 0.f, 0.f, 0.f };

	_float						m_fAlpha = { 1.f };
private:
	_float4						m_vInitColor = { 1.f, 1.f, 1.f, 1.f };
	_float4						m_vNowColor = { 1.f, 1.f, 1.f, 1.f };
	_float3						m_vGravity = { 0.f, 0.f, 0.f };

private:
	CVIBuffer_Particle* 		m_pVIBuffer_Particle = { nullptr };
	CTexture*					m_pBaseTexture = { nullptr };
	CTexture*					m_pMaskTexture = { nullptr };
	CTexture*					m_pNoiseTexture = { nullptr };
	CTexture*					m_pEmissiveTexture = { nullptr };

	CShader*					m_pShader = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_GlobalVariables();
	HRESULT Bind_ShaderResources();

public:
	static CToolEffect_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;


	friend class CEffect_Tool;
};

END

