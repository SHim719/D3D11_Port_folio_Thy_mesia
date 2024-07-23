#include "Effect_Particle.h"

CEffect_Particle::CEffect_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffect_Particle::CEffect_Particle(const CEffect_Particle& rhs)
	: CGameObject(rhs)
{

}

HRESULT CEffect_Particle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Particle::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Particle::Tick(_float fTimeDelta)
{
	//for (size_t i = 0; i < m_NowParticleDatas.size(); ++i)
	//{
	//	_vector vVelocity = XMLoadFloat3(&m_NowParticleDescs[i].vVelocity);
	//	_vector vForce = XMLoadFloat3(&m_NowParticleDescs[i].vForce);
	//
	//	_float4x4 TransformMatrix4x4;
	//	memcpy(&TransformMatrix4x4, &m_NowParticleDatas[i], sizeof(_float4x4));
	//
	//	_vector vPosition = XMLoadFloat4(&m_NowParticleDatas[i].vTranslation) + vVelocity * fTimeDelta;
	//
	//	_float fScaleX = XMVector3Length(XMLoadFloat4((_float4*)&TransformMatrix4x4.m[0])).m128_f32[0];
	//	_float fScaleY = XMVector3Length(XMLoadFloat4((_float4*)&TransformMatrix4x4.m[1])).m128_f32[1];
	//
	//	fScaleX = clamp(fScaleX + m_vScaleSpeed.x * fTimeDelta, 0.f, m_vMaxScale.x);
	//	fScaleY = clamp(fScaleY + m_vScaleSpeed.y * fTimeDelta, 0.f, m_vMaxScale.y);
	//
	//	if (m_bOrientToVelocity)
	//	{
	//		_matrix OrientMatrix = JoMath::OrientToDir(vVelocity);
	//
	//		OrientMatrix.r[0] = XMVector3Normalize(OrientMatrix.r[0]) * fScaleX;
	//		OrientMatrix.r[1] = XMVector3Normalize(OrientMatrix.r[1]) * fScaleY;
	//
	//		memcpy(&m_NowParticleDatas[i], &OrientMatrix, sizeof(_float4x4));
	//	}
	//	else
	//	{
	//		m_NowParticleDescs[i].vRotation.x += m_vRotationSpeed.x * fTimeDelta;
	//		m_NowParticleDescs[i].vRotation.y += m_vRotationSpeed.y * fTimeDelta;
	//		m_NowParticleDescs[i].vRotation.z += m_vRotationSpeed.z * fTimeDelta;
	//
	//		_matrix RotationMatrix = XMMatrixRotationRollPitchYaw(m_NowParticleDescs[i].vRotation.x
	//			, m_NowParticleDescs[i].vRotation.y, m_NowParticleDescs[i].vRotation.z);
	//
	//		RotationMatrix.r[0] = XMVector3Normalize(RotationMatrix.r[0]) * fScaleX;
	//		RotationMatrix.r[1] = XMVector3Normalize(RotationMatrix.r[1]) * fScaleY;
	//
	//		memcpy(&m_NowParticleDatas[i], &RotationMatrix, sizeof(_float4x4));
	//	}
	//
	//	XMStoreFloat4(&m_NowParticleDatas[i].vTranslation, vPosition);
	//
	//	vVelocity += vForce * fTimeDelta;
	//	XMStoreFloat3(&m_NowParticleDescs[i].vVelocity, vVelocity);
	//
	//	m_NowParticleDescs[i].fLifeTime -= fTimeDelta;
	//	if (m_NowParticleDescs[i].fLifeTime <= 0.f && m_bLoop)
	//		memcpy(&m_NowParticleDescs[i], &m_InitParticleDescs[i], sizeof(PARTICLE_DESC));
	//}
	//
	//if (m_pVIBuffer_Particle)
		//m_pVIBuffer_Particle->Update_Particle(m_NowParticleDatas);
}

void CEffect_Particle::LateTick(_float fTimeDelta)
{
	if (!m_bNoRender)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
}

HRESULT CEffect_Particle::Render()
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

HRESULT CEffect_Particle::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Particle"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer_Particle)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Diffuse"), TEXT("MaskTexture"), (CComponent**)&m_pMaskTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Masking"), TEXT("MaskTexture"), (CComponent**)&m_pMaskTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_Particle"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	return S_OK;
}


CEffect_Particle* CEffect_Particle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Particle* pInstance = new CEffect_Particle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_Particle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Particle::Clone(void* pArg)
{
	CEffect_Particle* pInstance = new CEffect_Particle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEffect_Particle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Particle::Free()
{
	__super::Free();
}
