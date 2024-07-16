#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Stats final : public CUI
{
private:
	enum StatInfo { LEVEL, SOUL, NEED_SOUL, STRENGTH, VITALITY, PLAGUE, DAMAGE, CLAW_DAMAGE, HP, INFO_END};

private:
	CUI_Stats(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Stats(const CUI_Stats& rhs);
	virtual ~CUI_Stats() = default;

	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	HRESULT OnEnter_Layer(void* pArg)	override;

private:
	HRESULT Render_BG();
	HRESULT Render_Highlighted();
	HRESULT Render_StatArrow();
	HRESULT Render_Fonts();
	HRESULT Render_Number(const string& strNumber, _fvector vRenderStartPos);

private:
	class CPlayerStats*		m_pPlayerStats = { nullptr };

	_float4x4				m_BGWorldMatrix = {};
	_float4x4				m_HighlightedMatrix = {};

	_float4					m_vNumberPos[INFO_END][2] = {};
	string					m_strNumbers[INFO_END][2] = {};

	_int					m_iOriginStatInfos[INFO_END] = {};
	_int					m_iStatInfos[INFO_END] = {};

	_bool					m_bDecideWindow = { false };

	_float4					m_vHighlightedPos[4] = {};
	_int					m_iSelectInfo = { 0 };
private:
	CTexture*	m_pNumberTexture = { nullptr };
	CTexture*	m_pHighlightedTexture = { nullptr };
	CTexture*	m_pStatArrowsTexture = { nullptr };

	CTexture* m_pArrowRightTexture = { nullptr };
private:
	HRESULT Ready_Component();
	void Init_UIMatrices();
	void Init_HighlightedPos();
	void Init_FontPos();
	void Init_StrFont();

public:
	static CUI_Stats* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

