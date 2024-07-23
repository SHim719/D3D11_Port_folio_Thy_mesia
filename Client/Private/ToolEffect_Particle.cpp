#include "ToolEffect_Particle.h"

CToolEffect_Particle::CToolEffect_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CToolEffect_Particle::CToolEffect_Particle(const CToolEffect_Particle& rhs)
	: CGameObject(rhs)
{

}

HRESULT CToolEffect_Particle::Initialize_Prototype()
{
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
	Update_TextureFlag();

	for (size_t i = 0; i < m_NowParticleDatas.size(); ++i)
	{
		Update_Positions(i, fTimeDelta);
		Update_Scale(i);
		Update_Velocity(i);
		Update_Color(i);

		m_NowParticleDatas[i].fLifeTime -= fTimeDelta;
		if (m_NowParticleDatas[i].fLifeTime <= 0.f)
		{
			m_NowParticleDatas[i].fLifeTime = 0.f; 
			if (m_bLoop)
				Restart_Particle();
		}	
	}

	if (m_pVIBuffer_Particle)
		m_pVIBuffer_Particle->Update_Particle(m_NowParticleDatas);
}

void CToolEffect_Particle::Update_TextureFlag()
{
	m_vTextureFlag.x = _float(m_iNoiseTextureIdx != -1);
	m_vTextureFlag.y = _float(m_iMaskTextureIdx != -1);
	m_vTextureFlag.z = _float(m_iEmissiveTextureIdx != -1);
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
	if (false == m_bSizeLerp || 0.f == m_InitParticleDatas[iIdx].fLifeTime)
		return;

	_float4x4 TransformMatrix4x4;
	memcpy(&TransformMatrix4x4, &m_NowParticleDatas[iIdx], sizeof(_float4x4));

	_vector vRight = XMLoadFloat4((_float4*)&TransformMatrix4x4.m[0]);
	_vector vUp = XMLoadFloat4((_float4*)&TransformMatrix4x4.m[1]);
	_float fSizeX = XMVector3Length(vRight).m128_f32[0];
	_float fSizeY = XMVector3Length(vUp).m128_f32[0];

	_float fLerpRatio = (1.f - m_NowParticleDatas[iIdx].fLifeTime / m_InitParticleDatas[iIdx].fLifeTime);
	fSizeX = JoMath::Lerp(fSizeX, m_vSizeEnd.x, fLerpRatio);
	fSizeY = JoMath::Lerp(fSizeY, m_vSizeEnd.y, fLerpRatio);

	vRight = XMVector3Normalize(vRight) * fSizeX;
	vUp = XMVector3Normalize(vUp) * fSizeY;

	XMStoreFloat4(&m_NowParticleDatas[iIdx].vRight, vRight);
	XMStoreFloat4(&m_NowParticleDatas[iIdx].vUp, vUp);
}

void CToolEffect_Particle::Update_Velocity(size_t iIdx)
{
	_vector vVelocity = XMLoadFloat3(&m_NowParticleDatas[iIdx].vVelocity);

	if (m_bSpeedLerp)
	{
		_float fLerpRatio = (1.f - m_NowParticleDatas[iIdx].fLifeTime / m_InitParticleDatas[iIdx].fLifeTime);

		_vector vDir = XMVector3Normalize(vVelocity);
		_float fLerpedSpeed = JoMath::Lerp(m_InitParticleDescs[iIdx].fSpeed, m_NowParticleDescs[iIdx].fSpeed, fLerpRatio);

		vVelocity = XMLoadFloat3(&m_NowParticleDescs[iIdx].vDir) * fLerpedSpeed;
	}
	else
	{
		vVelocity += XMVectorSet(0.f, -1.f, 0.f, 0.f) * m_fGravityScale;
	}

	XMStoreFloat3(&m_NowParticleDatas[iIdx].vVelocity, vVelocity);
}

void CToolEffect_Particle::Update_Color(size_t iIdx)
{
	if (m_bColorLerp)
	{
		_vector vInitColor = XMLoadFloat4(&m_vInitColor);

		_float fLerpRatio = (1.f - m_NowParticleDatas[iIdx].fLifeTime / m_InitParticleDatas[iIdx].fLifeTime);

		_vector vNowColor = XMVectorLerp(vInitColor, XMLoadFloat4(&m_vColorEnd), fLerpRatio);

		XMStoreFloat4(&m_vNowColor, vNowColor);
	}
}

void CToolEffect_Particle::LateTick(_float fTimeDelta)
{
	if (!m_bNoRender)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_EFFECT, this);
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

void CToolEffect_Particle::Restart_Particle()
{
	memcpy(m_NowParticleDatas.data(), m_InitParticleDatas.data(), sizeof(VTXPARTICLE) * m_InitParticleDatas.size());
	memcpy(m_NowParticleDescs.data(), m_InitParticleDescs.data(), sizeof(PARTICLE_DESC) * m_InitParticleDescs.size());

	m_vNowColor = m_vInitColor;

}

void CToolEffect_Particle::Remake_Particle()
{
	for (size_t i = 0; i < m_NowParticleDatas.size(); ++i)
	{
		VTXPARTICLE VtxParticle;

		_float2 vRandScale = JoRandom::Random_Float2(m_vStartSizeMin, m_vStartSizeMax);

		_matrix TransformMatrix = XMMatrixIdentity();

		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]) * vRandScale.x;
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]) * vRandScale.y;

		TransformMatrix.r[3] = XMVectorSetW(XMLoadFloat3(&JoRandom::Random_Float3(m_vStartPosMin, m_vStartPosMax)), 1.f);

		memcpy(&VtxParticle, &TransformMatrix, sizeof(_float4x4));

		VtxParticle.fLifeTime = JoRandom::Random_Float(m_fLifeTimeMin, m_fLifeTimeMax);

		memcpy(&m_NowParticleDatas[i], &VtxParticle, sizeof(VTXPARTICLE));
		memcpy(&m_InitParticleDatas[i], &VtxParticle, sizeof(VTXPARTICLE));

	}

	for (size_t i = 0; i < m_NowParticleDatas.size(); ++i)
	{
		_vector vDir = XMVector3Normalize(XMLoadFloat3(&JoRandom::Random_Float3(m_vSpeedDirMin, m_vSpeedDirMax)));
		_float fStartSpeed = JoRandom::Random_Float(m_fStartSpeedMin, m_fStartSpeedMax);

		_vector vVelocity = vDir * fStartSpeed;

		XMStoreFloat3(&m_InitParticleDescs[i].vDir, vDir);
		m_InitParticleDescs[i].fSpeed = fStartSpeed;

		XMStoreFloat3(&m_InitParticleDatas[i].vVelocity, vVelocity);
	
		memcpy(&m_NowParticleDescs[i], &m_InitParticleDescs[i], sizeof(PARTICLE_DESC));
	}

	m_vInitColor = m_vNowColor = JoRandom::Random_Float4(m_vStartColorMin, m_vStartColorMax);
	
	_vector vGravity = XMVectorSet(0.f, -1.f, 0.f, 0.f) * m_fGravityScale;
	XMStoreFloat3(&m_vGravity, vGravity);
}


void CToolEffect_Particle::Resize_Particles(_uint iNumParticles)
{
	if (0 == iNumParticles)
		return;

	m_pVIBuffer_Particle->Init_InstanceBuffer(iNumParticles);

	m_iNumParticles = iNumParticles;

	m_NowParticleDatas.clear();
	m_NowParticleDescs.clear();
	m_InitParticleDatas.clear();
	m_InitParticleDescs.clear();

	m_InitParticleDatas.resize(iNumParticles);
	m_NowParticleDatas.resize(iNumParticles);
	m_InitParticleDescs.resize(iNumParticles);
	m_NowParticleDescs.resize(iNumParticles);

	_float4x4 IdentityMatrix;
	XMStoreFloat4x4(&IdentityMatrix, XMMatrixIdentity());

	Remake_Particle();
	Restart_Particle();
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Noise"), TEXT("EmissiveTexture"), (CComponent**)&m_pEmissiveTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_Particle"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolEffect_Particle::Bind_GlobalVariables()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vColor_Mul", &m_vColorMul, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vColor_Offset", &m_vColorOffset, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vColor_Clip", &m_vClipColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_bBillBoard", &m_bBillBoard, sizeof(_bool))))
		return E_FAIL;

	if (m_bBillBoard)
	{
		if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
			return E_FAIL;
	}

	if (FAILED(m_pShader->Set_RawValue("g_vTextureFlag", &m_vTextureFlag, sizeof(_float4))))
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


}

