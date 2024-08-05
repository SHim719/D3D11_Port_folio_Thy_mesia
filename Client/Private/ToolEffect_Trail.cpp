#include "ToolEffect_Trail.h"

CToolEffect_Trail::CToolEffect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameEffect(pDevice, pContext)
{

}

CToolEffect_Trail::CToolEffect_Trail(const CToolEffect_Trail& rhs)
	: CGameEffect(rhs)
{
}
HRESULT CToolEffect_Trail::Initialize_Prototype(const wstring& wstrFilePath)
{
	m_eEffectType = TRAIL;

	if (wstrFilePath.empty())
		return S_OK;

	ifstream fin(wstrFilePath);

	if (FAILED(Load_EffectData(fin)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolEffect_Trail::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CToolEffect_Trail::Tick(_float fTimeDelta)
{
	m_fTrailUpdateAcc += fTimeDelta;
	if (m_fTrailUpdateAcc >= m_fTrailUpdateTime)
	{
		if (nullptr != m_pParentBone && nullptr != m_pParentTransform)
		{
			CALC_TF->Attach_To_Bone(m_pParentBone, m_pParentTransform);
			XMStoreFloat4x4(&m_ParentMatrix, CALC_TF->Get_WorldMatrix());
			m_pTrailBuffer->Update(XMLoadFloat4x4(&m_ParentMatrix));
		}
		m_fTrailUpdateAcc = 0.f;
	}
}

void CToolEffect_Trail::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_EFFECT_BLEND, this);

	if (m_bGlow)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_GLOW, this);
}

HRESULT CToolEffect_Trail::Render()
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

HRESULT CToolEffect_Trail::Save_EffectData(ofstream& fout)
{
	__super::Save_EffectData(fout);

	fout.write((_char*)&m_vColor, sizeof(_float4));
	fout.write((_char*)&m_fTrailUpdateTime, sizeof(_float));
	fout.write((_char*)&m_vStartPos, sizeof(_float3));
	fout.write((_char*)&m_vEndPos, sizeof(_float3));
	fout.write((_char*)&m_iMaxVertexCount, sizeof(_uint));
	fout.write((_char*)&m_iCatmullRomCount, sizeof(_uint));
	fout.write((_char*)&m_iRemoveCount, sizeof(_uint));

	return S_OK;
}

HRESULT CToolEffect_Trail::Load_EffectData(ifstream& fin)
{
	__super::Load_EffectData(fin);

	fin.read((_char*)&m_vColor, sizeof(_float4));
	fin.read((_char*)&m_fTrailUpdateTime, sizeof(_float));
	fin.read((_char*)&m_vStartPos, sizeof(_float3));
	fin.read((_char*)&m_vEndPos, sizeof(_float3));
	fin.read((_char*)&m_iMaxVertexCount, sizeof(_uint));
	fin.read((_char*)&m_iCatmullRomCount, sizeof(_uint));
	fin.read((_char*)&m_iRemoveCount, sizeof(_uint));

	Update_Buffer();

	return S_OK;
}


void CToolEffect_Trail::Update_Buffer()
{
	TRAIL_DESC TrailDesc{};

	TrailDesc.vStartPos = m_vStartPos;
	TrailDesc.vEndPos = m_vEndPos;
	TrailDesc.iMaxVertexCount = m_iMaxVertexCount;
	TrailDesc.iCatmullRomCount = m_iCatmullRomCount;
	TrailDesc.iRemoveCount = m_iRemoveCount;

	m_pTrailBuffer->Remake_Buffers(TrailDesc);
}

void CToolEffect_Trail::Reset_Points()
{
	m_pTrailBuffer->Reset_Points();
}


HRESULT CToolEffect_Trail::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Masking"), TEXT("MaskTexture"), (CComponent**)&m_pMaskTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Noise"), TEXT("NoiseTexture"), (CComponent**)&m_pNoiseTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_BaseColor"), TEXT("BaseTexture"), (CComponent**)&m_pBaseTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_EffectTrail"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	TRAIL_DESC TrailDesc{};

	TrailDesc.vStartPos = m_vStartPos;
	TrailDesc.vEndPos = m_vEndPos;
	TrailDesc.iMaxVertexCount = m_iMaxVertexCount;
	TrailDesc.iCatmullRomCount = m_iCatmullRomCount;
	TrailDesc.iRemoveCount = m_iRemoveCount;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Trail"), TEXT("VIBuffer"), (CComponent**)&m_pTrailBuffer, &TrailDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolEffect_Trail::Bind_GlobalVariables()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vColor_Clip", &m_vClipColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vTextureFlag", &m_vTextureFlag, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolEffect_Trail::Bind_ShaderResources()
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

CToolEffect_Trail* CToolEffect_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolEffect_Trail* pInstance = new CToolEffect_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(L"")))
	{
		MSG_BOX(TEXT("Failed To Created : CToolEffect_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CToolEffect_Trail* CToolEffect_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& wstrFilePath)
{
	CToolEffect_Trail* pInstance = new CToolEffect_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(wstrFilePath)))
	{
		MSG_BOX(TEXT("Failed To Created : CToolEffect_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToolEffect_Trail::Clone(void* pArg)
{
	CToolEffect_Trail* pInstance = new CToolEffect_Trail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CToolEffect_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolEffect_Trail::Free()
{
	__super::Free();

	Safe_Release(m_pTrailBuffer);
}
