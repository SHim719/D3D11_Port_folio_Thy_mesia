#include "Effect_Trail.h"

CEffect_Trail::CEffect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameEffect(pDevice, pContext)
{

}

CEffect_Trail::CEffect_Trail(const CEffect_Trail& rhs)
	: CGameEffect(rhs)
	, m_vColor(rhs.m_vColor)
	, m_fTrailUpdateTime(rhs.m_fTrailUpdateTime)
	, m_tTrailDesc(rhs.m_tTrailDesc)


{
}
HRESULT CEffect_Trail::Initialize_Prototype(ifstream& fin)
{
	m_eEffectType = TRAIL;

	return Load_EffectData(fin);
}

HRESULT CEffect_Trail::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_VIBuffer_Trail", TEXT("Trail_Buffer"), (CComponent**)&m_pTrailBuffer, &m_tTrailDesc)))
		return E_FAIL;

	Update_TextureFlag();

	return S_OK;
}

void CEffect_Trail::Tick(_float fTimeDelta)
{
	m_fTrailUpdateAcc += fTimeDelta;

}

void CEffect_Trail::LateTick(_float fTimeDelta)
{
	if (m_fTrailUpdateAcc >= m_fTrailUpdateTime)
	{
		m_fTrailUpdateAcc = 0.f;
		m_pTrailBuffer->Update(XMLoadFloat4x4(&m_ParentMatrix));
	}

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_EFFECT_BLEND, this);

	if (m_bGlow)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_GLOW, this);
}

HRESULT CEffect_Trail::Render()
{
	if (FAILED(Bind_GlobalVariables()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pTrailBuffer->Bind_Buffers()))
		return E_FAIL;

	return m_pTrailBuffer->Render();
}

HRESULT CEffect_Trail::Load_EffectData(ifstream& fin)
{
	__super::Load_EffectData(fin);

	fin.read((_char*)&m_vColor, sizeof(_float4));
	fin.read((_char*)&m_fTrailUpdateTime, sizeof(_float));
	fin.read((_char*)&m_tTrailDesc, sizeof(TRAIL_DESC));

	return S_OK;
}

HRESULT CEffect_Trail::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Masking"), TEXT("MaskTexture"), (CComponent**)&m_pMaskTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Noise"), TEXT("NoiseTexture"), (CComponent**)&m_pNoiseTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_BaseColor"), TEXT("BaseTexture"), (CComponent**)&m_pBaseTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_EffectTrail"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Trail::Bind_GlobalVariables()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
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

	return S_OK;
}

HRESULT CEffect_Trail::Bind_ShaderResources()
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

	return S_OK;

}

CEffect_Trail* CEffect_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& fin)
{
	CEffect_Trail* pInstance = new CEffect_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(fin)))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Trail::Clone(void* pArg)
{
	CEffect_Trail* pInstance = new CEffect_Trail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEffect_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Trail::Free()
{
	__super::Free();

	Safe_Release(m_pTrailBuffer);
}
