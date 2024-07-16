#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_PlayerBar final : public CUI
{
private:
	enum PLAYERBAR { LEFT, BG, RIGHT, PLAYERBAR_END };

private:
	CUI_PlayerBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PlayerBar(const CUI_PlayerBar& rhs);
	virtual ~CUI_PlayerBar() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	HRESULT Render()					override;

	void Draw_HpText();
	void Update_PlayerHp(_int iMaxHp, _int iHp);
private:
	_float4					m_vHpRenderStartPos = {};
	_float3					m_vHpBarScales[PLAYERBAR_END] = {};

	_float2					m_vHpTextRenderPos = {};

	class CProgressBar*		m_pHpProgressBar = { nullptr };
	CTexture*				m_pPlayerHpBarTex[PLAYERBAR_END] = {};

private:
	_int					m_iPlayerMaxHp = { 0 };
	_int					m_iPlayerNowHp = { 0 };

private:
	HRESULT Render_HpBar();
	

private:
	HRESULT Ready_Component();
	HRESULT Init_HpBar();

public:
	static CUI_PlayerBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

