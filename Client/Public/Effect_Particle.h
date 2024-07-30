#pragma once

#include "Client_Defines.h"
#include "GameEffect.h"

BEGIN(Client)
class CEffect_Particle final : public CGameEffect
{
public:
	enum PARTICLE_TYPE { BILLBOARD, ALIGNVELOCITY, SPAWN_AT_BONE, PARTICLE_END };

private:
	CEffect_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Particle(const CEffect_Particle& rhs);
	virtual ~CEffect_Particle() = default;

private:
	HRESULT Initialize_Prototype(ifstream& fin);
	HRESULT Initialize(void* pArg)						override;

	void Tick(_float fTimeDelta)						override;
	void LateTick(_float fTimeDelta)					override;
	HRESULT Render()									override;

private:
	HRESULT Load_EffectData(ifstream& fin)			override;

private:
	_bool Update_SpawnTime(size_t iIdx, _float fTimeDelta);
	void Update_Positions(size_t iIdx, _float fTimeDelta);
	void Update_Scale(size_t iIdx);
	void Update_Rotation(size_t iIdx, _float fTimeDelta);
	void Update_Velocity(size_t iIdx, _float fTimeDelta);
	void Update_Color(size_t iIdx);
	void Update_Lifetime(size_t iIdx, _float fTimeDelta);

	void Update_BoneMatrix();
public:
	void Restart_Effect(EFFECTSPAWNDESC* pDesc = nullptr)	override;
	void Restart_Particle(size_t iIdx);
	void Remake_Particle();
	void Resize_Particles(_uint iNumParticles);

private:
	vector<VTXPARTICLE>			m_InitParticleDatas;
	vector<VTXPARTICLE>			m_NowParticleDatas;

	vector<PARTICLE_DESC>		m_InitParticleDescs;
	vector<PARTICLE_DESC>		m_NowParticleDescs;

	PARTICLE_INFO				m_tParticleInfo = {};

	_float4x4					m_BoneMatrix = {};
private:
	CVIBuffer_Particle*			m_pVIBuffer_Particle = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_GlobalVariables();
	HRESULT Bind_ShaderResources();

public:
	static CEffect_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& fin);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END

