#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_PotionSlot final : public CUI
{
private:
	CUI_PotionSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PotionSlot(const CUI_PotionSlot& rhs);
	virtual ~CUI_PotionSlot() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	HRESULT Render()					override;
	HRESULT Render_PotionCount();

public:
	void Set_PotionCount(_int iPotionCount) {
		m_iPotionCount = iPotionCount;
		m_wstrPotionText = to_wstring(m_iPotionCount) + L" / " + to_wstring(m_iMaxPotionCount);
	}

private:
	_int m_iPotionCount = { 5 };
	_int m_iMaxPotionCount = { 5 };

	wstring m_wstrPotionText = L"5 / 5";
	_float2 m_vPotionTextPos = {};

private:
	HRESULT Ready_Component();

public:
	static CUI_PotionSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

