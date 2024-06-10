#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CRenderTarget : public CBase
{
public:
	typedef struct tagRenderTargetDesc
	{
		_uint iWinSizeX = 0;
		_uint iWinSizeY = 0;
		DXGI_FORMAT eFormat = DXGI_FORMAT_UNKNOWN;
	}RT_DESC;

private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

	HRESULT Initialize(void* pArg);

public:
	void Clear_RTV(_vector vClearColor);

protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

	ID3D11Texture2D*			m_pTexture = nullptr;
	ID3D11ShaderResourceView*	m_pSRV = nullptr;
	ID3D11RenderTargetView*		m_pRTV = nullptr;

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;

};
END
