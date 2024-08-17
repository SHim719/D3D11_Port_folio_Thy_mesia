#include "Aisemy.h"

#include "LightObject.h"

CAisemy::CAisemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CAisemy::CAisemy(const CAisemy& rhs)
	: CGameObject(rhs)
{
}

HRESULT CAisemy::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAisemy::Initialize(void* pArg)
{
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_fCullingRadius = 2.f;
	m_fDissolveSpeed = -0.4f;
	m_fDissolveAmount = 1.f;
	m_bDissolve = true;

	LIGHT_DESC LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vAmbient = { 0.f, 1.f, 0.6f, 1.f };
	LightDesc.vDiffuse = { 0.f, 1.f, 0.6f, 1.f };
	LightDesc.fRange = 2.5f;
	LightDesc.fLightStrength = 1.5f;

	m_pLight = static_cast<CLightObject*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Light", L"Prototype_LightObject", &LightDesc));

	return S_OK;
}

void CAisemy::Tick(_float fTimeDelta)
{
	Update_Dissolve(fTimeDelta);

	Update_UV(fTimeDelta);

	m_pLight->Set_LightPosition(m_pTransform->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f));
}

void CAisemy::LateTick(_float fTimeDelta)
{
	if (m_pGameInstance->In_WorldFrustum(m_pTransform->Get_Position(), m_fCullingRadius))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_EFFECT_NONBLEND, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_GLOW, this);
	}
}

HRESULT CAisemy::Render()
{
	if (FAILED(Bind_GlobalVariables()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pModel->Bind_Buffers(0)))
		return E_FAIL;
	
	if (FAILED(m_pModel->Render(m_pShader, 0, 3)))
		return E_FAIL;

	return S_OK;
}

void CAisemy::Update_UV(_float fTimeDelta)
{
	m_vNoiseUVOffset.x += m_vNoiseUVSpeed.x * fTimeDelta;
	m_vNoiseUVOffset.y += m_vNoiseUVSpeed.y * fTimeDelta;

	if (fabsf(m_vNoiseUVOffset.x) >= 9999.f)
		m_vNoiseUVOffset.x = 0.f;
	if (fabsf(m_vNoiseUVOffset.y) >= 9999.f)
		m_vNoiseUVOffset.y = 0.f;
}


HRESULT CAisemy::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_BaseColor"), TEXT("BaseTexture"), (CComponent**)&m_pBaseTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Noise"), TEXT("NoiseTexture"), (CComponent**)&m_pNoiseTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Dissolve"), TEXT("DissolveTexture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModel"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Aisemy"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAisemy::Bind_GlobalVariables()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vNoiseUVOffset", &m_vNoiseUVOffset, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_fDissolveAmount", &m_fDissolveAmount, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CAisemy::Bind_ShaderResources()
{
	if (FAILED(m_pBaseTexture->Set_SRV(m_pShader, "g_BaseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pNoiseTexture->Set_SRV(m_pShader, "g_NoiseTexture", 8)))
		return E_FAIL;

	if (FAILED(m_pDissolveTexture->Set_SRV(m_pShader, "g_DissolveTexture")))
		return E_FAIL;

	return S_OK;
}


CAisemy* CAisemy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAisemy* pInstance = new CAisemy(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CAisemy"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAisemy::Clone(void* pArg)
{
	CAisemy* pInstance = new CAisemy(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CAisemy"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAisemy::Free()
{
	__super::Free();

	Safe_Release(m_pBaseTexture);
	Safe_Release(m_pNoiseTexture);
	Safe_Release(m_pDissolveTexture);
	
	Safe_Release(m_pModel);
	Safe_Release(m_pShader);
}

