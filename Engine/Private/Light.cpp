#include "..\Public\Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CLight::CLight()
{
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer* pVIBuffer)
{
	_uint		iPassIndex = { 0 };

	if (FAILED(pShader->Set_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(pShader->Set_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(pShader->Set_RawValue("g_fLightStrength", &m_LightDesc.fLightStrength, sizeof(_float))))
		return E_FAIL;

	switch (m_LightDesc.eType)
	{
	case LIGHT_DESC::TYPE_DIRECTIONAL:
		if (FAILED(Bind_Directional(pShader, iPassIndex)))
			return E_FAIL;
		break;

	case LIGHT_DESC::TYPE_POINT:
		if (FAILED(Bind_Point(pShader, iPassIndex)))
			return E_FAIL;
		break;
	}

	if (FAILED(pShader->Begin(iPassIndex + 1)))
		return E_FAIL;

	return pVIBuffer->Render();
}

HRESULT CLight::Bind_Directional(CShader* pShader, OUT _uint& iPassIdx)
{
	iPassIdx = LIGHT_DESC::TYPE_DIRECTIONAL;

	if (FAILED(pShader->Set_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLight::Bind_Point(CShader* pShader, OUT _uint& iPassIdx)
{
	iPassIdx = LIGHT_DESC::TYPE_POINT;

	if (FAILED(pShader->Set_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(pShader->Set_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CLight* CLight::Create(const LIGHT_DESC& LightDesc)
{
	CLight* pInstance = new CLight();

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CLight"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	__super::Free();

}
