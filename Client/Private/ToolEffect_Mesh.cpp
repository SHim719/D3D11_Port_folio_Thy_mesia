#include "ToolEffect_Mesh.h"

CToolEffect_Mesh::CToolEffect_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CToolEffect_Mesh::CToolEffect_Mesh(const CToolEffect_Mesh& rhs)
	: CGameObject(rhs)
{
}

HRESULT CToolEffect_Mesh::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CToolEffect_Mesh::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CToolEffect_Mesh::Tick(_float fTimeDelta)
{
	Update_Rotation(fTimeDelta);
	Update_Scale(fTimeDelta);
	Update_UVMask(fTimeDelta);
	Update_UVNoise(fTimeDelta);
}

void CToolEffect_Mesh::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
}

HRESULT CToolEffect_Mesh::Render()
{
	if (nullptr == m_pModel)
		return S_OK;

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(Bind_ShaderResoruces()))
		return E_FAIL;

	if (FAILED(m_pModel->Bind_Buffers(0)))
		return E_FAIL;

	if (FAILED(m_pModel->Render(m_pShader, 0, 0)))
		return E_FAIL;

	return S_OK;
}

void CToolEffect_Mesh::Init_Attributes()
{

}

void CToolEffect_Mesh::Update_Rotation(_float fTimeDelta)
{
	if (m_vRotationSpeed.x != 0.f)
		m_pTransform->Add_RollInput(m_vRotationSpeed.x * fTimeDelta);
	if (m_vRotationSpeed.y != 0.f)
		m_pTransform->Add_YawInput(m_vRotationSpeed.y * fTimeDelta);
	if (m_vRotationSpeed.z != 0.f)
		m_pTransform->Add_PitchInput(m_vRotationSpeed.z * fTimeDelta);
	
}

void CToolEffect_Mesh::Update_Scale(_float fTimeDelta)
{
	_float3 vNowScale = m_pTransform->Get_Scale();

	vNowScale.x += m_vScaleSpeed.x * fTimeDelta;
	vNowScale.y += m_vScaleSpeed.y * fTimeDelta;
	vNowScale.z += m_vScaleSpeed.z * fTimeDelta;

	m_pTransform->Set_Scale(vNowScale);
}

void CToolEffect_Mesh::Update_UVMask(_float fTimeDelta)
{
	m_vUVMask.x += m_vUVMaskSpeed.x * fTimeDelta;
	m_vUVMask.y += m_vUVMaskSpeed.y * fTimeDelta;

	if (fabsf(m_vUVMask.x) >= 999999.f)
		m_vUVMask.x = 0.f;
	if (fabsf(m_vUVMask.y) >= 999999.f)
		m_vUVMask.y = 0.f;
}

void CToolEffect_Mesh::Update_UVNoise(_float fTimeDelta)
{
	m_vUVNoise.x += m_vUVNoiseSpeed.x * fTimeDelta;
	m_vUVNoise.y += m_vUVNoiseSpeed.y * fTimeDelta;

	if (fabsf(m_vUVNoise.x) >= 999999.f)
		m_vUVNoise.x = 0.f;
	if (fabsf(m_vUVNoise.y) >= 999999.f)
		m_vUVNoise.y = 0.f;
}

HRESULT CToolEffect_Mesh::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Masking"), TEXT("MaskTexture"), (CComponent**)&m_pMaskTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Noise"), TEXT("NoiseTexture"), (CComponent**)&m_pNoiseTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_FadeScreen"), TEXT("DiffuseTexture"), (CComponent**)&m_pDiffuseTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_MeshEffect"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;


	return S_OK;
}

HRESULT CToolEffect_Mesh::Bind_ShaderResoruces()
{
	if (FAILED(m_pDiffuseTexture->Set_SRV(m_pShader, "g_DiffuseTexture", m_iDiffuseTextureIdx)))
		return E_FAIL;

	_bool bUseMask = { false };
	if (m_iMaskTextureIdx >= 0)
	{
		bUseMask = true;

		if (FAILED(m_pMaskTexture->Set_SRV(m_pShader, "g_MaskTexture", m_iMaskTextureIdx)))
			return E_FAIL;

		if (FAILED(m_pShader->Set_RawValue("g_vMaskUV", &m_vUVMask, sizeof(_float2))))
			return E_FAIL;

	}
	
	_bool bUseNoise = { false };
	if (m_iNoiseTextureIdx >= 0)
	{
		bUseNoise = true;
		if (FAILED(m_pNoiseTexture->Set_SRV(m_pShader, "g_NoiseTexture", m_iNoiseTextureIdx)))
			return E_FAIL;

		if (FAILED(m_pShader->Set_RawValue("g_vNoiseUV", &m_vUVNoise, sizeof(_float2))))
			return E_FAIL;
	}

	if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;
		
	return S_OK;
}

CToolEffect_Mesh* CToolEffect_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolEffect_Mesh* pInstance = new CToolEffect_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CToolEffect_Mesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToolEffect_Mesh::Clone(void* pArg)
{
	CToolEffect_Mesh* pInstance = new CToolEffect_Mesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CToolEffect_Mesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolEffect_Mesh::Free()
{
	Safe_Release(m_pShader);
	Safe_Release(m_pMaskTexture);
	Safe_Release(m_pNoiseTexture);
	Safe_Release(m_pDissolveTexture);

}
