#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_DamageFont final : public CUI
{
public:
	typedef struct tagDamageFontDesc
	{
		_float4 vWorldPosition = {};
		string	strDamage = "";
	} DAMAGEFONTDESC;

private:
	CUI_DamageFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_DamageFont(const CUI_DamageFont& rhs);
	virtual ~CUI_DamageFont() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	string		m_strDamage = "";

	_float4		m_vRenderStartPos = {};
	_float		m_fFontGapX = { 17.f };

	_float		m_fDeltaAlphaSpeed = { 0.4f };
	_float		m_fFontMoveSpeed = { 80.f };

private:
	HRESULT Ready_Component();

public:
	static CUI_DamageFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

