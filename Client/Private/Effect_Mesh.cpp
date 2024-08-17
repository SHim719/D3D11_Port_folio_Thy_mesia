#include "Effect_Mesh.h"

CEffect_Mesh::CEffect_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameEffect(pDevice, pContext)
{

}

CEffect_Mesh::CEffect_Mesh(const CEffect_Mesh& rhs)
	: CGameEffect(rhs)
	, m_tMeshEffectInfo(rhs.m_tMeshEffectInfo)
{
}
HRESULT CEffect_Mesh::Initialize_Prototype(ifstream& fin)
{
	m_eEffectType = MESH;

	if (FAILED(Load_EffectData(fin)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Mesh::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Mesh::Tick(_float fTimeDelta)
{
	if (m_bComplete)
		return;

	if (false == Update_SpawnTime(fTimeDelta))
		return;

	Update_Rotation(fTimeDelta);
	Update_Scale();
	Update_Color();
	Update_UV(fTimeDelta);
	Update_LifeTime(fTimeDelta);

	Update_ClipRange(fTimeDelta);

	Update_FinalMatrix();
}

void CEffect_Mesh::LateTick(_float fTimeDelta)
{
	if (m_bComplete || m_bNoRender)
		return;

	m_pGameInstance->Add_RenderObject((CRenderer::RENDERGROUP)m_iRenderGroup, this);

	if (m_bGlow)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_GLOW, this);

	if (m_tMeshEffectInfo.bDistortion)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DISTORTION, this);
}

HRESULT CEffect_Mesh::Render()
{
	if (nullptr == m_pModel)
		return S_OK;

	if (FAILED(Bind_GlobalVariables()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pModel->Bind_Buffers(0)))
		return E_FAIL;

	if (FAILED(m_pModel->Render(m_pShader, 0, m_iPassIdx)))
		return E_FAIL;

	return S_OK;
}


HRESULT CEffect_Mesh::Load_EffectData(ifstream& fin)
{
	__super::Load_EffectData(fin);

	fin.read((_char*)&m_tMeshEffectInfo, sizeof(MESHEFFECT_INFO));

	return S_OK;
}


void CEffect_Mesh::Restart_Effect(EFFECTSPAWNDESC* pDesc)
{
	__super::Restart_Effect(pDesc);

	m_bComplete = false;
	m_fSpawnTimeAcc = m_tMeshEffectInfo.fSpawnTime;
	m_bNoRender = true;
	m_fClipRange = 0.f;

	m_tMeshEffectInfo.vColor = m_tMeshEffectInfo.vStartColor;
	m_vMaskUVOffset = m_tMeshEffectInfo.vStartMaskUVOffset;
	m_vNoiseUVOffset = m_tMeshEffectInfo.vStartNoiseUVOffset;
	m_pLocalTransform->Set_WorldMatrix(XMMatrixIdentity());
	m_pLocalTransform->Set_Position(XMVectorSetW(XMLoadFloat3(&m_tMeshEffectInfo.vStartPosition), 1.f));
	m_pLocalTransform->Rotation_RollPitchYaw(m_tMeshEffectInfo.vStartRotation.x, m_tMeshEffectInfo.vStartRotation.y, m_tMeshEffectInfo.vStartRotation.z);
	m_pLocalTransform->Set_Scale(m_tMeshEffectInfo.vStartScale);
}

void CEffect_Mesh::OnEnd_Effect()
{
	__super::OnEnd_Effect();
}

_bool CEffect_Mesh::Update_SpawnTime(_float fTimeDelta)
{
	if (m_fSpawnTimeAcc >= 0.f)
	{
		m_fSpawnTimeAcc -= fTimeDelta;
		if (m_fSpawnTimeAcc < 0.f)
		{
			m_bNoRender = false;
			Spawn_Effect();
			return true;
		}
			
		return false;
	}
	else
		return true;
}

void CEffect_Mesh::Update_Rotation(_float fTimeDelta)
{
	if (m_tMeshEffectInfo.bRotationLerp)
	{
		if (0.f == m_fPlayTime)
			return;

		_float fRatio = m_fTimeAcc / m_fPlayTime;
		_vector vLerpedRotation = XMVectorLerp(XMLoadFloat3(&m_tMeshEffectInfo.vStartRotation), XMLoadFloat3(&m_tMeshEffectInfo.vRotationEnd), fRatio);
		m_pLocalTransform->Rotation_RollPitchYaw(To_Radian(vLerpedRotation.m128_f32[0]), To_Radian(vLerpedRotation.m128_f32[1]), To_Radian(vLerpedRotation.m128_f32[2]));
	}
	else
	{
		if (0.f == m_tMeshEffectInfo.fTurnSpeed)
			return;

		_vector vAxis;
		if (m_tMeshEffectInfo.bLocal)
			vAxis = XMVector3Normalize(m_pLocalTransform->Get_Right() * m_tMeshEffectInfo.vRotationAxis.x + m_pLocalTransform->Get_Up() * m_tMeshEffectInfo.vRotationAxis.y + m_pLocalTransform->Get_Look() * m_tMeshEffectInfo.vRotationAxis.z);
		else
			vAxis = XMVector3Normalize(XMVectorSet(1.f, 0.f, 0.f, 0.f) * m_tMeshEffectInfo.vRotationAxis.x
				+ XMVectorSet(0.f, 1.f, 0.f, 0.f) * m_tMeshEffectInfo.vRotationAxis.y + (XMVectorSet(0.f, 0.f, 1.f, 0.f) * m_tMeshEffectInfo.vRotationAxis.z));

		m_pLocalTransform->Turn(vAxis, fTimeDelta);
	}

}

void CEffect_Mesh::Update_Scale()
{
	if (m_tMeshEffectInfo.bScaleLerp)
	{
		if (0.f == m_fPlayTime)
			return;

		_float fRatio = m_fTimeAcc / m_fPlayTime;
		_vector vLerpedScale = XMVectorLerp(XMLoadFloat3(&m_tMeshEffectInfo.vStartScale), XMLoadFloat3(&m_tMeshEffectInfo.vScaleEnd), fRatio);

		_float3 vScale;
		XMStoreFloat3(&vScale, vLerpedScale);

		m_pLocalTransform->Set_Scale(vScale);
	}
}

void CEffect_Mesh::Update_Color()
{
	if (m_tMeshEffectInfo.bColorLerp)
	{
		if (0.f == m_fPlayTime)
			return;

		_float fRatio = m_fTimeAcc / m_fPlayTime;
		XMStoreFloat4(&m_tMeshEffectInfo.vColor, XMVectorLerp(XMLoadFloat4(&m_tMeshEffectInfo.vStartColor), XMLoadFloat4(&m_tMeshEffectInfo.vColorEnd), fRatio));
	}

}

void CEffect_Mesh::Update_UV(_float fTimeDelta)
{
	m_vMaskUVOffset.x += m_tMeshEffectInfo.vMaskUVSpeed.x * fTimeDelta;
	m_vMaskUVOffset.y += m_tMeshEffectInfo.vMaskUVSpeed.y * fTimeDelta;

	if (1 == m_tMeshEffectInfo.iMaskSampler) // Clamp
	{
		m_vMaskUVOffset.x = clamp(m_vMaskUVOffset.x, m_tMeshEffectInfo.vMinMaskUVOffset.x, m_tMeshEffectInfo.vMaxMaskUVOffset.x);
		m_vMaskUVOffset.y = clamp(m_vMaskUVOffset.y, m_tMeshEffectInfo.vMinMaskUVOffset.y, m_tMeshEffectInfo.vMaxMaskUVOffset.y);
	}
	else // Wrap
	{
		if (fabsf(m_vMaskUVOffset.x) >= 9999.f)
			m_vMaskUVOffset.x = 0.f;
		if (fabsf(m_vMaskUVOffset.y) >= 9999.f)
			m_vMaskUVOffset.y = 0.f;
	}


	m_vNoiseUVOffset.x += m_tMeshEffectInfo.vNoiseUVSpeed.x * fTimeDelta;
	m_vNoiseUVOffset.y += m_tMeshEffectInfo.vNoiseUVSpeed.y * fTimeDelta;

	if (1 == m_tMeshEffectInfo.iNoiseSampler) // Clamp
	{
		m_vNoiseUVOffset.x = clamp(m_vNoiseUVOffset.x, m_tMeshEffectInfo.vMinNoiseUVOffset.x, m_tMeshEffectInfo.vMaxNoiseUVOffset.x);
		m_vNoiseUVOffset.y = clamp(m_vNoiseUVOffset.y, m_tMeshEffectInfo.vMinNoiseUVOffset.y, m_tMeshEffectInfo.vMaxNoiseUVOffset.y);
	}
	else
	{
		if (fabsf(m_vNoiseUVOffset.x) >= 9999.f)
			m_vNoiseUVOffset.x = 0.f;
		if (fabsf(m_vNoiseUVOffset.y) >= 9999.f)
			m_vNoiseUVOffset.y = 0.f;
	}

}

void CEffect_Mesh::Update_LifeTime(_float fTimeDelta)
{
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

void CEffect_Mesh::Update_FinalMatrix()
{
	Update_World();

	_matrix LocalMatrix = m_pLocalTransform->Get_WorldMatrix();
	_matrix WorldMatrix = XMMatrixIdentity();

	if (m_pTransform)
		WorldMatrix = m_pTransform->Get_WorldMatrix();

	_matrix FinalMatrix = LocalMatrix * WorldMatrix;

	XMStoreFloat4x4(&m_FinalMatrix, XMMatrixTranspose(FinalMatrix));
}

void CEffect_Mesh::Update_ClipRange(_float fTimeDelta)
{
	if (1 != m_iPassIdx)
		return;

	m_fClipRange += 0.5f * fTimeDelta;
}

HRESULT CEffect_Mesh::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Local_Transform"), (CComponent**)&m_pLocalTransform)))
		return E_FAIL;
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Masking"), TEXT("MaskTexture"), (CComponent**)&m_pMaskTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Noise"), TEXT("NoiseTexture"), (CComponent**)&m_pNoiseTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_BaseColor"), TEXT("BaseTexture"), (CComponent**)&m_pBaseTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_MeshEffect"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;


	wstring wstrModelTag = m_tMeshEffectInfo.szModelTag;
	if (wstrModelTag.empty())
		return S_OK;

	wstrModelTag = L"Prototype_Model_" + wstrModelTag;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, wstrModelTag, L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Mesh::Bind_GlobalVariables()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_FinalMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vColor", &m_tMeshEffectInfo.vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vColor_Clip", &m_vClipColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vTextureFlag", &m_vTextureFlag, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vMaskUVOffset", &m_vMaskUVOffset, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vNoiseUVOffset", &m_vNoiseUVOffset, sizeof(_float2))))
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

	if (FAILED(m_pShader->Set_RawValue("g_bDistortion", &m_tMeshEffectInfo.bDistortion, sizeof(_bool))))
		return E_FAIL;

	if (m_tMeshEffectInfo.bDistortion)
	{
		if (FAILED(m_pShader->Set_RawValue("g_fDistortionIntensity", &m_tMeshEffectInfo.fDistortion_Intensity, sizeof(_float))))
			return E_FAIL;
	}

	if (1 == m_iPassIdx)
	{
		if (FAILED(m_pShader->Set_RawValue("g_fClipRange", &m_fClipRange, sizeof(_float))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Mesh::Bind_ShaderResources()
{
	if (FAILED(m_pBaseTexture->Set_SRV(m_pShader, "g_BaseTexture", m_iBaseTextureIdx)))
		return E_FAIL;

	if (-1 != m_iMaskTextureIdx)
	{
		if (FAILED(m_pShader->Set_RawValue("g_iMaskSampler", &m_tMeshEffectInfo.iMaskSampler, sizeof(_int))))
			return E_FAIL;

		if (FAILED(m_pMaskTexture->Set_SRV(m_pShader, "g_MaskTexture", m_iMaskTextureIdx)))
			return E_FAIL;
	}

	if (-1 != m_iNoiseTextureIdx)
	{
		if (FAILED(m_pShader->Set_RawValue("g_iNoiseSampler", &m_tMeshEffectInfo.iNoiseSampler, sizeof(_int))))
			return E_FAIL;

		if (FAILED(m_pNoiseTexture->Set_SRV(m_pShader, "g_NoiseTexture", m_iNoiseTextureIdx)))
			return E_FAIL;
	}


	return S_OK;

}

CEffect_Mesh* CEffect_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& fin)
{
	CEffect_Mesh* pInstance = new CEffect_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(fin)))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_Mesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Mesh::Clone(void* pArg)
{
	CEffect_Mesh* pInstance = new CEffect_Mesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEffect_Mesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Mesh::Free()
{
	__super::Free();

	Safe_Release(m_pLocalTransform);
	Safe_Release(m_pModel);
}
