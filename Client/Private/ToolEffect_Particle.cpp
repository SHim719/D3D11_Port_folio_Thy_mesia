#include "ToolEffect_Particle.h"

CToolEffect_Particle::CToolEffect_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameEffect(pDevice, pContext)
{
}

CToolEffect_Particle::CToolEffect_Particle(const CToolEffect_Particle& rhs)
	: CGameEffect(rhs)
	, m_tParticleInfo(rhs.m_tParticleInfo)
{

}

HRESULT CToolEffect_Particle::Initialize_Prototype()
{
	m_eEffectType = EFFECTTYPE::PARTICLE;
	return S_OK;
}

HRESULT CToolEffect_Particle::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	Resize_Particles(1);

	return S_OK;
}

void CToolEffect_Particle::Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < m_NowParticleDatas.size(); ++i)
	{
		if (false == Update_SpawnTime(i, fTimeDelta))
			continue;

		if (m_NowParticleDatas[i].fLifeTime <= 0.f)
			continue;

		Update_Positions(i, fTimeDelta);
		Update_Scale(i);
		Update_Rotation(i, fTimeDelta);
		Update_Velocity(i, fTimeDelta);
		Update_Color(i);
		Update_Lifetime(i, fTimeDelta);
	}

	Update_World();

	if (m_pVIBuffer_Particle)
		m_pVIBuffer_Particle->Update_Particle(m_NowParticleDatas);

	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc >= m_fPlayTime)
	{
		m_fTimeAcc = 0.f;
		if (!m_bLoop)
			m_bComplete = true;
		else
			Restart_Effect();
	}
}

_bool CToolEffect_Particle::Update_SpawnTime(size_t iIdx, _float fTimeDelta)
{
	if (m_NowParticleDescs[iIdx].fSpawnTime >= 0.f)
	{
		m_NowParticleDatas[iIdx].fLifeTime = 0.f;
		m_NowParticleDescs[iIdx].fSpawnTime -= fTimeDelta;
		if (m_NowParticleDescs[iIdx].fSpawnTime < 0.f)
		{
			memcpy(&m_NowParticleDatas[iIdx], &m_InitParticleDatas[iIdx], sizeof(VTXPARTICLE));
			if (m_pParentBone && m_tParticleInfo.iParticleMode & (1 << SPAWN_AT_BONE))
			{
				memcpy(&m_NowParticleDatas[iIdx], &m_InitParticleDatas[iIdx], sizeof(VTXPARTICLE));

				_matrix ParticleMatrix = XMLoadFloat4x4((_float4x4*)&m_InitParticleDatas[iIdx]);
				_matrix BoneMatrix = XMLoadFloat4x4(&m_BoneMatrix);
				CALC_TF->Set_WorldMatrix(ParticleMatrix * BoneMatrix);
				//_float3 vStartScale = CALC_TF->Get_Scale();

				//CALC_TF->Set_WorldMatrix(XMLoadFloat4x4(&m_BoneMatrix));
				//CALC_TF->Set_Scale(vStartScale);

				memcpy(&m_NowParticleDatas[iIdx], &CALC_TF->Get_WorldFloat4x4(), sizeof(_float4x4));
			}
			return true;
		}
		else
		{
			return false;
		}
		
	}
	else
	{
		return true;
	}
}

void CToolEffect_Particle::Update_Positions(size_t iIdx, _float fTimeDelta)
{
	_float4x4 TransformMatrix4x4;
	memcpy(&TransformMatrix4x4, &m_NowParticleDatas[iIdx], sizeof(_float4x4));

	_vector vVelocity = XMLoadFloat3(&m_NowParticleDatas[iIdx].vVelocity);
	_vector vPosition = XMLoadFloat4(&m_NowParticleDatas[iIdx].vTranslation) + vVelocity * fTimeDelta;

	XMStoreFloat4(&m_NowParticleDatas[iIdx].vTranslation, vPosition);
}

void CToolEffect_Particle::Update_Scale(size_t iIdx)
{
	if (false == m_tParticleInfo.bSizeLerp || 0.f == m_InitParticleDatas[iIdx].fLifeTime)
		return;

	_float4x4 TransformMatrix4x4;
	memcpy(&TransformMatrix4x4, &m_NowParticleDatas[iIdx], sizeof(_float4x4));

	_vector vRight = XMLoadFloat4((_float4*)&TransformMatrix4x4.m[0]);
	_vector vUp = XMLoadFloat4((_float4*)&TransformMatrix4x4.m[1]);
	_float fSizeX = XMVector3Length(XMLoadFloat4(&m_InitParticleDatas[iIdx].vRight)).m128_f32[0];
	_float fSizeY = XMVector3Length(XMLoadFloat4(&m_InitParticleDatas[iIdx].vUp)).m128_f32[1];

	_float fLerpRatio = (1.f - m_NowParticleDatas[iIdx].fLifeTime / m_InitParticleDatas[iIdx].fLifeTime);
	fSizeX = JoMath::Lerp(fSizeX, m_tParticleInfo.vSizeEnd.x, fLerpRatio);
	fSizeY = JoMath::Lerp(fSizeY, m_tParticleInfo.vSizeEnd.y, fLerpRatio);

	vRight = XMVector3Normalize(vRight) * fSizeX;
	vUp = XMVector3Normalize(vUp) * fSizeY;

	XMStoreFloat4(&m_NowParticleDatas[iIdx].vRight, vRight);
	XMStoreFloat4(&m_NowParticleDatas[iIdx].vUp, vUp);
}

void CToolEffect_Particle::Update_Rotation(size_t iIdx, _float fTimeDelta)
{
	if (0.f == m_NowParticleDescs[iIdx].vRotationSpeed.x && 0.f == m_NowParticleDescs[iIdx].vRotationSpeed.y && 0.f == m_NowParticleDescs[iIdx].vRotationSpeed.z)
		return;

	_float4x4 ParticleMatrix;
	memcpy(&ParticleMatrix, &m_NowParticleDatas[iIdx], sizeof(_float4x4));

	CALC_TF->Set_WorldMatrix(XMLoadFloat4x4(&ParticleMatrix));

	CALC_TF->Add_RollInput(m_NowParticleDescs[iIdx].vRotationSpeed.x * fTimeDelta);
	CALC_TF->Add_YawInput(m_NowParticleDescs[iIdx].vRotationSpeed.y * fTimeDelta);
	CALC_TF->Add_PitchInput(m_NowParticleDescs[iIdx].vRotationSpeed.z * fTimeDelta);

	memcpy(&m_NowParticleDatas[iIdx], &CALC_TF->Get_WorldMatrix(), sizeof(_float4x4));
}

void CToolEffect_Particle::Update_Velocity(size_t iIdx, _float fTimeDelta)
{
	_vector vVelocity = XMLoadFloat3(&m_NowParticleDatas[iIdx].vVelocity);

	if (m_tParticleInfo.bSpeedLerp)
	{
		_float fLerpRatio = (1.f - m_NowParticleDatas[iIdx].fLifeTime / m_InitParticleDatas[iIdx].fLifeTime);

		_vector vDir = XMVector3Normalize(vVelocity);
		_float fLerpedSpeed = JoMath::Lerp(m_InitParticleDescs[iIdx].fSpeed, m_tParticleInfo.fSpeedEnd, fLerpRatio);

		vVelocity = XMLoadFloat3(&m_NowParticleDescs[iIdx].vSpeedDir) * fLerpedSpeed;
	}
	else
	{
		vVelocity += XMLoadFloat3(&m_NowParticleDescs[iIdx].vForceDir) * m_NowParticleDescs[iIdx].fForceScale * fTimeDelta;
	}

	XMStoreFloat3(&m_NowParticleDatas[iIdx].vVelocity, vVelocity);
}

void CToolEffect_Particle::Update_Color(size_t iIdx)
{
	if (m_tParticleInfo.bColorLerp)
	{
		_float fLerpRatio = (1.f - m_NowParticleDatas[iIdx].fLifeTime / m_InitParticleDatas[iIdx].fLifeTime);

		_vector vNowColor = XMVectorLerp(XMLoadFloat4(&m_InitParticleDatas[iIdx].vColor), XMLoadFloat4(&m_tParticleInfo.vColorEnd), fLerpRatio);

		XMStoreFloat4(&m_NowParticleDatas[iIdx].vColor, vNowColor);
	}
}

void CToolEffect_Particle::Update_Lifetime(size_t iIdx, _float fTimeDelta)
{
	m_NowParticleDatas[iIdx].fLifeTime -= fTimeDelta;
	if (m_NowParticleDatas[iIdx].fLifeTime <= 0.f)
	{
		m_NowParticleDatas[iIdx].fLifeTime = 0.f;
		if (m_tParticleInfo.bParticleLoop) 
			Restart_Particle(iIdx);
	}
}


void CToolEffect_Particle::Update_BoneMatrix()
{
	if (nullptr == m_pParentBone || !(m_tParticleInfo.iParticleMode & (1 << SPAWN_AT_BONE)))
		return;

	CALC_TF->Attach_To_Bone(m_pParentBone, m_pParentTransform);

	XMStoreFloat4x4(&m_BoneMatrix, CALC_TF->Get_WorldMatrix());
}

void CToolEffect_Particle::LateTick(_float fTimeDelta)
{
	Update_BoneMatrix();
	
	if (!m_bNoRender)
	{
		m_pGameInstance->Add_RenderObject((CRenderer::RENDERGROUP)m_iRenderGroup, this);
		if (m_bGlow)
			m_pGameInstance->Add_RenderObject(CRenderer::RENDER_GLOW, this);
	}

}

HRESULT CToolEffect_Particle::Render()
{
	if (FAILED(Bind_GlobalVariables()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin((_uint)m_iPassIdx)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer_Particle->Bind_Buffers()))
		return E_FAIL;

	return m_pVIBuffer_Particle->Render();
}

HRESULT CToolEffect_Particle::Save_EffectData(ofstream& fout)
{
	__super::Save_EffectData(fout);

	fout.write((_char*)&m_tParticleInfo, sizeof(PARTICLE_INFO));

	return S_OK;
}

HRESULT CToolEffect_Particle::Load_EffectData(ifstream& fin)
{
	__super::Load_EffectData(fin);

	fin.read((_char*)&m_tParticleInfo, sizeof(PARTICLE_INFO));

	Resize_Particles(m_tParticleInfo.iNumParticles);

	return S_OK;
}

void CToolEffect_Particle::Restart_Effect(EFFECTSPAWNDESC* pDesc)
{
	__super::Restart_Effect(pDesc);

	Spawn_Effect();
	//memcpy(m_NowParticleDatas.data(), m_InitParticleDatas.data(), sizeof(VTXPARTICLE) * m_InitParticleDatas.size());
	memcpy(m_NowParticleDescs.data(), m_InitParticleDescs.data(), sizeof(PARTICLE_DESC) * m_InitParticleDescs.size());

	Update_BoneMatrix();

	m_bComplete = false;
}

void CToolEffect_Particle::Restart_Particle(size_t iIdx)
{
	memcpy(&m_NowParticleDescs[iIdx], &m_InitParticleDescs[iIdx], sizeof(PARTICLE_DESC));

}

void CToolEffect_Particle::Remake_Particle()
{
	for (size_t i = 0; i < m_NowParticleDatas.size(); ++i)
	{
		VTXPARTICLE VtxParticle;

		_float2 vRandScale = JoRandom::Random_Float2(m_tParticleInfo.vStartSizeMin, m_tParticleInfo.vStartSizeMax);
		_float3 vRandRotation = JoRandom::Random_Float3(m_tParticleInfo.vStartRotationMin, m_tParticleInfo.vStartRotationMax);
		_vector vQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&vRandRotation));

		_matrix TransformMatrix = XMMatrixRotationQuaternion(vQuat);

		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]) * vRandScale.x;
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]) * vRandScale.y;

		TransformMatrix.r[3] = XMVectorSetW(XMLoadFloat3(&JoRandom::Random_Float3(m_tParticleInfo.vStartPosMin, m_tParticleInfo.vStartPosMax)), 1.f);

		memcpy(&VtxParticle, &TransformMatrix, sizeof(_float4x4));

		VtxParticle.fLifeTime = JoRandom::Random_Float(m_tParticleInfo.fLifeTimeMin, m_tParticleInfo.fLifeTimeMax);
		VtxParticle.vColor = JoRandom::Random_Float4(m_tParticleInfo.vStartColorMin, m_tParticleInfo.vStartColorMax);

		memcpy(&m_NowParticleDatas[i], &VtxParticle, sizeof(VTXPARTICLE));
		memcpy(&m_InitParticleDatas[i], &VtxParticle, sizeof(VTXPARTICLE));
		m_NowParticleDatas[i].fLifeTime = 0.f;
	}

	for (size_t i = 0; i < m_NowParticleDatas.size(); ++i)
	{
		_vector vDir = XMVector3Normalize(XMLoadFloat3(&JoRandom::Random_Float3(m_tParticleInfo.vSpeedDirMin, m_tParticleInfo.vSpeedDirMax)));
		_float fStartSpeed = JoRandom::Random_Float(m_tParticleInfo.fStartSpeedMin, m_tParticleInfo.fStartSpeedMax);

		_vector vVelocity = vDir * fStartSpeed;

		XMStoreFloat3(&m_InitParticleDescs[i].vSpeedDir, vDir);
		m_InitParticleDescs[i].fSpeed = fStartSpeed;

		m_InitParticleDescs[i].fSpawnTime = JoRandom::Random_Float(m_tParticleInfo.fSpawnTimeMin, m_tParticleInfo.fSpawnTimeMax);

		XMStoreFloat3(&m_InitParticleDatas[i].vVelocity, vVelocity);

		_vector vForceDir = XMVector3Normalize(XMLoadFloat3(&JoRandom::Random_Float3(m_tParticleInfo.vForceDirMin, m_tParticleInfo.vForceDirMax)));
		XMStoreFloat3(&m_InitParticleDescs[i].vForceDir, vForceDir);

		_float fForceScale = JoRandom::Random_Float(m_tParticleInfo.fForceScaleMin, m_tParticleInfo.fForceScaleMax);
		m_InitParticleDescs[i].fForceScale = fForceScale;

		XMStoreFloat3(&m_InitParticleDescs[i].vForceDir, vForceDir);
		memcpy(&m_NowParticleDescs[i], &m_InitParticleDescs[i], sizeof(PARTICLE_DESC));

		m_InitParticleDescs[i].vRotationSpeed = JoRandom::Random_Float3(m_tParticleInfo.vRotationSpeedMin, m_tParticleInfo.vRotationSpeedMax);
	}
}


void CToolEffect_Particle::Resize_Particles(_uint iNumParticles)
{
	if (0 == iNumParticles)
		return;

	m_pVIBuffer_Particle->Init_InstanceBuffer(iNumParticles);

	m_tParticleInfo.iNumParticles = iNumParticles;

	m_NowParticleDatas.clear();
	m_NowParticleDescs.clear();
	m_InitParticleDatas.clear();
	m_InitParticleDescs.clear();

	m_InitParticleDatas.resize(iNumParticles);
	m_NowParticleDatas.resize(iNumParticles);
	m_InitParticleDescs.resize(iNumParticles);
	m_NowParticleDescs.resize(iNumParticles);

	Remake_Particle();
}

HRESULT CToolEffect_Particle::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Particle"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer_Particle)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_BaseColor"), TEXT("BaseTexture"), (CComponent**)&m_pBaseTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Masking"), TEXT("MaskTexture"), (CComponent**)&m_pMaskTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Noise"), TEXT("NoiseTexture"), (CComponent**)&m_pNoiseTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_Particle"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolEffect_Particle::Bind_GlobalVariables()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vColor_Clip", &m_vClipColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vTextureFlag", &m_vTextureFlag, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_bGlow", &m_bGlow, sizeof(_bool))))
		return E_FAIL;

	if (m_bGlow)
	{
		if (FAILED(m_pShader->Set_RawValue("g_vGlowColor", &m_vGlowColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShader->Set_RawValue("g_fGlowIntensity", &m_fGlowIntensity, sizeof(_float))))
			return E_FAIL;
	}

	if (FAILED(m_pShader->Set_RawValue("g_bBloom", &m_bBloom, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_iParticleMode", &m_tParticleInfo.iParticleMode, sizeof(_int))))
		return E_FAIL;

	if (m_tParticleInfo.iParticleMode & (1 << BILLBOARD))
		if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
			return E_FAIL;

	return S_OK;
}


HRESULT CToolEffect_Particle::Bind_ShaderResources()
{
	if (FAILED(m_pBaseTexture->Set_SRV(m_pShader, "g_BaseTexture", m_iBaseTextureIdx)))
		return E_FAIL;


	if (-1 != m_iMaskTextureIdx)
	{
		if (FAILED(m_pMaskTexture->Set_SRV(m_pShader, "g_MaskTexture", m_iMaskTextureIdx)))
			return E_FAIL;
	}

	if (-1 != m_iNoiseTextureIdx)
	{
		if (FAILED(m_pNoiseTexture->Set_SRV(m_pShader, "g_NoiseTexture", m_iNoiseTextureIdx)))
			return E_FAIL;
	}

	if (-1 != m_iEmissiveTextureIdx)
	{
		if (FAILED(m_pNoiseTexture->Set_SRV(m_pShader, "g_EmissiveTexture", m_iEmissiveTextureIdx)))
			return E_FAIL;
	}

	return S_OK;
}


CToolEffect_Particle* CToolEffect_Particle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolEffect_Particle* pInstance = new CToolEffect_Particle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CToolEffect_Particle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToolEffect_Particle::Clone(void* pArg)
{
	CToolEffect_Particle* pInstance = new CToolEffect_Particle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CToolEffect_Particle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolEffect_Particle::Free()
{
	__super::Free();

	Safe_Release(m_pVIBuffer_Particle);
}

