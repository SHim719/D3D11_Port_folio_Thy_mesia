#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CEffect_Particle final : public CGameObject
{
private:
	CEffect_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Particle(const CEffect_Particle& rhs);
	virtual ~CEffect_Particle() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;

	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;

	HRESULT Render()					override;


private:
	vector<VTXPARTICLE>			m_InitParticleDatas;
	vector<VTXPARTICLE>			m_NowParticleDatas;

	vector<PARTICLE_DESC>		m_InitParticleDescs;
	vector<PARTICLE_DESC>		m_NowParticleDescs;

	_bool						m_bBillBoard = { false };
	_int						m_iNumParticles = { 0 };
	_int						m_iMaskTexureIdx = { 0 };
	_int						m_iPassIdx = { 0 };

	_bool						m_bTargeting = { false };
	_float3						m_fTargetPos = { 0.f, 0.f, 0.f };

	_float4						m_vColor = { 1.f, 1.f, 1.f, 1.f };

	_float2						m_vScaleSpeed = { 0.f, 0.f };

	_float2						m_vMaxScale = { 999.f, 999.f };

	_float3						m_vRotationSpeed = { 0.f, 0.f, 0.f };

	_bool						m_bOrientToVelocity = { false };
	_bool						m_bLoop = { false };

	_float						m_fLifeTime = { 1.f };
private:
	CVIBuffer_Particle* m_pVIBuffer_Particle = { nullptr };
	CTexture* m_pMaskTexture = { nullptr };
	CShader* m_pShader = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CEffect_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;


	friend class CEffect_Tool;
};

END

