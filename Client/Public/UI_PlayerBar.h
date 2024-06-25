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

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	void Update_PlayerHp(_int iMaxHp, _int iHp)		override;
	void Update_PlayerMp(_int iMaxMp, _int iMp)		override;

private:
	_float4					m_vHpRenderStartPos = {};
	_float3					m_vHpBarScales[PLAYERBAR_END] = {};

	class CProgressBar*		m_pHpProgressBar = { nullptr };
	CTexture*				m_pPlayerHpBarTex[PLAYERBAR_END] = {};

	_float4					m_vMpRenderStartPos = {};
	_float3					m_vMpBarScales[PLAYERBAR_END] = {};

	class CProgressBar*		m_pMpProgressBar = { nullptr };
	CTexture*				m_pPlayerMpBarTex[PLAYERBAR_END] = {};

private:
	_int					m_iPlayerMaxHp = { 0 };
	_int					m_iPlayerNowHp = { 0 };
	_int					m_iPlayerMaxMp = { 0 };
	_int					m_iPlayerNowMp = { 0 };

private:
	HRESULT Render_HpBar();
	HRESULT Render_MpBar();

	void Draw_HpText();
	void Draw_MpText();

private:
	HRESULT Ready_Component();
	HRESULT Init_HpBar();
	HRESULT Init_MpBar();

public:
	static CUI_PlayerBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

