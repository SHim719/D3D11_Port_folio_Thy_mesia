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
	for (size_t i = 0; i < m_NowParticleDatas.size(); ++i)
	{
		_vector vVelocity = XMLoadFloat3(&m_NowParticleDescs[i].vVelocity);
		_vector vForce = XMLoadFloat3(&m_NowParticleDescs[i].vForce);

		_float4x4 TransformMatrix4x4;
		memcpy(&TransformMatrix4x4, &m_NowParticleDatas[i], sizeof(_float4x4));

		_vector vPosition = XMLoadFloat4(&m_NowParticleDatas[i].vTranslation) + vVelocity * fTimeDelta;

		_float fScaleX = XMVector3Length(XMLoadFloat4((_float4*)&TransformMatrix4x4.m[0])).m128_f32[0];
		_float fScaleY = XMVector3Length(XMLoadFloat4((_float4*)&TransformMatrix4x4.m[1])).m128_f32[1];

		fScaleX = clamp(fScaleX + m_vScaleSpeed.x * fTimeDelta, 0.f, m_vMaxScale.x);
		fScaleY = clamp(fScaleY + m_vScaleSpeed.y * fTimeDelta, 0.f, m_vMaxScale.y);

		if (m_bOrientToVelocity)
		{
			_matrix OrientMatrix = JoMath::OrientToDir(vVelocity);

			OrientMatrix.r[0] = XMVector3Normalize(OrientMatrix.r[0]) * fScaleX;
			OrientMatrix.r[1] = XMVector3Normalize(OrientMatrix.r[1]) * fScaleY;

			memcpy(&m_NowParticleDatas[i], &OrientMatrix, sizeof(_float4x4));
		}
		else
		{
			m_NowParticleDescs[i].vRotation.x += m_vRotationSpeed.x * fTimeDelta;
			m_NowParticleDescs[i].vRotation.y += m_vRotationSpeed.y * fTimeDelta;
			m_NowParticleDescs[i].vRotation.z += m_vRotationSpeed.z * fTimeDelta;

			_matrix RotationMatrix = XMMatrixRotationRollPitchYaw(m_NowParticleDescs[i].vRotation.x
				, m_NowParticleDescs[i].vRotation.y, m_NowParticleDescs[i].vRotation.z);

			RotationMatrix.r[0] = XMVector3Normalize(RotationMatrix.r[0]) * fScaleX;
			RotationMatrix.r[1] = XMVector3Normalize(RotationMatrix.r[1]) * fScaleY;

			memcpy(&m_NowParticleDatas[i], &RotationMatrix, sizeof(_float4x4));
		}

		XMStoreFloat4(&m_NowParticleDatas[i].vTranslation, vPosition);

		vVelocity += vForce * fTimeDelta;
		XMStoreFloat3(&m_NowParticleDescs[i].vVelocity, vVelocity);

		m_NowParticleDescs[i].fLifeTime -= fTimeDelta;
		if (m_NowParticleDescs[i].fLifeTime <= 0.f && m_bLoop)
			memcpy(&m_NowParticleDescs[i], &m_InitParticleDescs[i], sizeof(PARTICLE_DESC));
	}

	if (m_pVIBuffer_Particle)
		m_pVIBuffer_Particle->Update_Particle(m_NowParticleDatas);
}

void CToolEffect_Particle::LateTick(_float fTimeDelta)
{
	if (!m_bNoRender)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_EFFECT, this);
}

HRESULT CToolEffect_Particle::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_bBillBoard", &m_bBillBoard, sizeof(_bool))))
		return E_FAIL;

	if (m_bBillBoard)
		m_pShader->Set_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4));

	if (FAILED(m_pMaskTexture->Set_SRV(m_pShader, "g_MaskTexture", m_iMaskTexureIdx)))
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

}

void CToolEffect_Particle::Remake_Particle()
{
	for (size_t i = 0; i < m_NowParticleDatas.size(); ++i)
	{
		VTXPARTICLE VtxParticle;

		_float2 vRandScale = JoRandom::Random_Float2(m_vScaleMin, m_vScaleMax);

		XMStoreFloat3(&m_InitParticleDescs[i].vRotation, XMVectorSet(JoRandom::Random_Float(m_vRotationMin.x, m_vRotationMax.x),
			JoRandom::Random_Float(m_vRotationMin.y, m_vRotationMax.y), JoRandom::Random_Float(m_vRotationMin.z, m_vRotationMax.z), 0.f));

		_matrix TransformMatrix = XMMatrixRotationRollPitchYaw(m_InitParticleDescs[i].vRotation.x,
			m_InitParticleDescs[i].vRotation.y, m_InitParticleDescs[i].vRotation.z);

		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]) * vRandScale.x;
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]) * vRandScale.y;

		TransformMatrix.r[3] = XMVectorSetW(XMLoadFloat3(&JoRandom::Random_Float3(m_vStartPosMin, m_vStartPosMax)), 1.f);

		memcpy(&VtxParticle, &TransformMatrix, sizeof(_float4x4));

		memcpy(&m_NowParticleDatas[i], &VtxParticle, sizeof(VTXPARTICLE));
		memcpy(&m_InitParticleDatas[i], &VtxParticle, sizeof(VTXPARTICLE));
	}

	for (size_t i = 0; i < m_NowParticleDatas.size(); ++i)
	{
		_vector vVelocity = XMVector3Normalize(XMLoadFloat3(&JoRandom::Random_Float3(m_vDirMin, m_vDirMax)))
			* JoRandom::Random_Float(m_fVelocityScaleMin, m_fVelocityScaleMax);

		XMStoreFloat3(&m_InitParticleDescs[i].vVelocity, vVelocity);

		_vector vForce = XMVector3Normalize(XMLoadFloat3(&JoRandom::Random_Float3(m_vForceDirMin, m_vForceDirMax)))
			* JoRandom::Random_Float(m_fForceScaleMin, m_fForceScaleMax);

		XMStoreFloat3(&m_InitParticleDescs[i].vForce, vForce);

		m_InitParticleDescs[i].fLifeTime = m_fLifeTime;

		memcpy(&m_NowParticleDescs[i], &m_InitParticleDescs[i], sizeof(PARTICLE_DESC));
	}
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
}

HRESULT CToolEffect_Particle::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Particle"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer_Particle)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Masking"), TEXT("MaskTexture"), (CComponent**)&m_pMaskTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_Particle"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

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

