#pragma once

#include "Base.h"

/* 1. ����Ÿ�ٵ��� ��� �ִ´�. */

/* 2. �갡 ��� �ִ� ����Ÿ�ٵ��� ��ġ�� ���ε����ִ� ��Ȱ. */
/* 2.1 ��ġ�� ���ÿ� ���ε� �Ǿ���� ����Ÿ�ٵ��� �̸� ����ΰڴ�.(�׷� == mrt) */

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
private:
	CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTarget_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderTarget(const wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixel, const _float4& vClearColor);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strRenderTargetTag);

	HRESULT Begin_MRT(const wstring& strMRTTag);
	HRESULT Begin_MRT_NoClear(const wstring& strMRTTag);
	HRESULT End_MRT();

	HRESULT Bind_RT_SRV(const wstring& strRenderTargetTag, class CShader* pShader, const _char* pConstantName);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Render_SingleTargetDebug(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	ID3D11RenderTargetView* m_pOldRTV = { nullptr };
	ID3D11DepthStencilView* m_pDSV = { nullptr };

private:
	map<const wstring, class CRenderTarget*>			m_RenderTargets;

private:
	map<const wstring, list<class CRenderTarget*>>		m_MRTs;

private:
	class CRenderTarget* Find_RenderTarget(const wstring& strRenderTargetTag);
	list<class CRenderTarget*>* Find_MRT(const wstring& strMRTTag);
public:
	static CTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END