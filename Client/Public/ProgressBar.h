#pragma once

#include "UI.h"

BEGIN(Client)

class CProgressBar final : public CUI
{
public:
	typedef struct tagProgressBarDesc
	{
		_float4 vPosition;
		_float3 vScale;
		wstring wstrTextureTag = L"";
	} PROGRESSBARDESC;

private:
	CProgressBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProgressBar(const CProgressBar& rhs);
	virtual ~CProgressBar() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	HRESULT Render()					override;

private:
	void Setting_World();

private:
	_float		m_fRatio = { 0.f };
	_float4		m_vCenter = {};
	_float		m_fLeftX = { 0.f };
	_float3		m_vOriginScale = {};

public:
	void Set_Ratio(_float fRatio) {
		m_fRatio = fRatio;
	}

private:
	HRESULT Ready_Component(const wstring& wstrTexTag);

public:
	static CProgressBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

