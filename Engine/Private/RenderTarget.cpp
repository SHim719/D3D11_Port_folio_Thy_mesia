#include "RenderTarget.h"

CRenderTarget::CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
}

HRESULT CRenderTarget::Initialize(void* pArg)
{
	RT_DESC* pRTDesc = (RT_DESC*)pArg;

	D3D11_TEXTURE2D_DESC TextureDesc = {};

	TextureDesc.Width = pRTDesc->iWidth;
	TextureDesc.Height = pRTDesc->iHeight;
	TextureDesc.Format = pRTDesc->eFormat;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture, nullptr, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture, nullptr, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}

void CRenderTarget::Clear_RTV(_vector vClearColor)
{
	if (nullptr == m_pContext)
		return;

	_float4 vColor;
	XMStoreFloat4(&vColor, vClearColor);

	m_pContext->ClearRenderTargetView(m_pRTV, (_float*)&vColor);
}

CRenderTarget* CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX(L"Failed to Created : CRenderTarget");
	}

	return pInstance;
}

void CRenderTarget::Free()
{
	Safe_Release(m_pSRV);
	Safe_Release(m_pRTV);
	Safe_Release(m_pTexture);
	
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
