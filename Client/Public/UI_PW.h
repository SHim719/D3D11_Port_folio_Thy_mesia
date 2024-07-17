#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_PW final : public CUI
{
private:
	enum Menu { LEVEL_UP, PLAGUE_WEAPON, RESUME, MENU_END };

private:
	CUI_PW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PW(const CUI_PW& rhs);
	virtual ~CUI_PW() = default;

	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;
private:
	void Select_Menu();

	HRESULT Render_BG();
	HRESULT Render_Highlighted();
	HRESULT Render_ThymesiaFont();
	HRESULT Render_Fonts();

private:
	Menu		m_eNowMenu = { LEVEL_UP };

	_float4x4	m_BGWorldMatrix = {};
	_float4x4	m_ThymesiaFontMatrix = {};
	_float4x4	m_FontMatrix[MENU_END] = {};

	_float4		m_vFontPos[MENU_END] = {};
private:
	CTexture* m_pMenuHighlightedTex = { nullptr };
	CTexture* m_pThymesiaFontTex = { nullptr };
	CTexture* m_pMenuFontTextures[MENU_END] = {};

private:
	HRESULT Ready_Component();
	void Init_UIMatrices();

	void Open_Window(const string& strUI);

public:
	static CUI_PW* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

