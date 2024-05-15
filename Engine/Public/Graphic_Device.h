#pragma once

#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{	
private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
	HRESULT Initialize(const GRAPHIC_DESC& GraphicDesc, _Out_ ID3D11Device** ppDeviceOut, _Out_ ID3D11DeviceContext** ppDeviceContextOut);
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

private:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;
	IDXGISwapChain*			m_pSwapChain;

	ID3D11RenderTargetView*	m_pBackBufferRTV;

	ID3D11Texture2D*		m_pDSBuffer;
	ID3D11DepthStencilView*	m_pDSView;
	
	_uint					m_iNumQualityLevel;

private:
	HRESULT Init_Device_And_SwapChain(const GRAPHIC_DESC& GraphicDesc);
	HRESULT Init_RTV();
	HRESULT Init_DepthStencil(const GRAPHIC_DESC& GraphicDesc);

public:
	static CGraphic_Device* Create(const GRAPHIC_DESC& GraphicDesc, _Out_ ID3D11Device** ppDeviceOut, _Out_ ID3D11DeviceContext** ppDeviceContextOut);
	virtual void Free() override;
};

END