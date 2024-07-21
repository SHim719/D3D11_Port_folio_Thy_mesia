#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Popup final : public CUI
{
private:
	enum POPUP_STATE { Appear, Wait, Disappear};

private:
	CUI_Popup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Popup(const CUI_Popup& rhs);
	virtual ~CUI_Popup() = default;

private:
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	HRESULT OnEnter_Layer(void* pArg)	override;

private:
	vector<CTexture*>	m_pPopupTextures = {};

	POPUP_STATE			m_ePopupState = { Appear };

	_float4				m_vRenderStartPos = {};
	_float				m_fRenderPosX = {};
	_float				m_fLerpRate = { 1.f };
	_float				m_fDeltaRateSpeed = { 3.f };

	_float				m_fWaitTime = { 2.f };
	_float				m_fTimeAcc = { 0.f };

private:
	HRESULT Ready_Component();

	wstring Get_PrototypeTag(SKILLTYPE ePopupType) const;

public:
	static CUI_Popup* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

