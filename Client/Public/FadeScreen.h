#pragma once

#include "UI.h"

BEGIN(Client)

class CFadeScreen final : public CUI
{
public:
	enum FADECOLOR { BLACK, WHITE, COLOR_END};
	enum FADETYPE { FADEOUT, FADEIN, FADETYPE_END};

public:
	typedef struct tagFadeDesc
	{
		FADETYPE			eFadeType;
		FADECOLOR			eFadeColor;
		_float				fAlphaDeltaSpeed;
		_float				fExtraTime = 0.f;
		function<void()>	Callback_FadeStart = nullptr;
		function<void()>	Callback_FadeEnd = nullptr;
	}FADEDESC;	

private:
	CFadeScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFadeScreen(const CFadeScreen& rhs);
	virtual ~CFadeScreen() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	_float		m_fAlpha = { 0.f };
	_float		m_fAlphaDeltaSpeed = { };
	_float		m_fExtraTime = { 0.f };
	_bool		m_bExtraTime = { false };

	FADETYPE	m_eFadeType = { FADETYPE_END };

	_float4		m_vFadeColor = {};

	function<void()> m_Callback_FadeStart = { nullptr };
	function<void()> m_Callback_FadeEnd = { nullptr };

public:
	static CFadeScreen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

