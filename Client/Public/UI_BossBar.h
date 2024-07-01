#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_BossBar final : public CUI
{
public:
	enum BOSSBAR { LEFT, BG, RIGHT, BOSSBAR_END };

private:
	CUI_BossBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BossBar(const CUI_BossBar& rhs);
	virtual ~CUI_BossBar() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	void Update_EnemyHp(_int iHp)		override;
	void Update_EnemyMp(_int iMp)		override;

	HRESULT OnEnter_Layer(void* pArg)	override;
private:
	void Draw_BossNameText();

private:
	wstring m_wstrBossName = L"";
	
private:
	HRESULT Render_MainBar();

private:
	_float4				m_vRenderStartPos = {};
	_float3				m_vBossBarScales[BOSSBAR_END] = {};


	CTexture*			m_pBossBarTex[BOSSBAR_END] = {};

	class CProgressBar* m_pHpProgressBar = { nullptr };
	class CProgressBar* m_pMpProgressBar = { nullptr };

	_int				m_iMaxHp = { 0 };
private:
	HRESULT Ready_Component();


public:
	static CUI_BossBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

