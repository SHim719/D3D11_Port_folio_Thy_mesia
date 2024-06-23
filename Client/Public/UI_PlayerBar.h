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

	void On_UIActive(void* pArg)		override;

	void Update_PlayerHp(_float fMaxHp, _float fHp)		override;
	void Update_PlayerMp(_int iMaxMp, _int iMp)			override;

private:
	_float4					m_vRenderStartPos = {};
	_float3					m_vHpBarScales[PLAYERBAR_END] = {};

	class CProgressBar*		m_pHpProgressBar = { nullptr };
	CTexture*				m_pPlayerHpBarTex[PLAYERBAR_END] = {};

private:
	HRESULT Ready_Component();
	
public:
	static CUI_PlayerBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

