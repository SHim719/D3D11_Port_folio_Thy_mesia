#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_SoulBar final : public CUI
{
private:
	enum SoulBar { BG, ICON, BAR_END};

private:
	CUI_SoulBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_SoulBar(const CUI_SoulBar& rhs);
	virtual ~CUI_SoulBar() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	HRESULT Render()					override;

	void Draw_SoulText();
private:
	void Init_Matrices();

private:
	_float	m_fSoulNumber = { 0.f };
	_float	m_fTargetSoulNumber = { 0.f };

	_float  m_fDeltaSpeed = { 400.f };
	_bool	m_bSoulUpdated = { false };

public:
	void Set_SoulNumber(_uint iSoulNumber);

private:
	CTexture*	m_Textures[BAR_END] = {};
	_float4x4	m_Matrices[BAR_END] = {};

	_float2		m_vFontPos = { 1228.f, 24.f };
	_float		m_fFontScaleX = 10.f;
private:
	HRESULT Ready_Component();

public:
	static CUI_SoulBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

