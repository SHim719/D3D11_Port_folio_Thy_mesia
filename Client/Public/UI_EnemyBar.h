#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_EnemyBar : public CUI
{
protected:
	enum ENEMYBAR { LEFT, BG, RIGHT, ENEMYBAR_END };

protected:
	CUI_EnemyBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_EnemyBar(const CUI_EnemyBar& rhs);
	virtual ~CUI_EnemyBar() = default;

protected:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	void Update_EnemyHp(_int iHp)		override;
	void Update_EnemyMp(_int iMp)		override;

protected:
	_float4					m_vRenderStartPos = {};
	_float3					m_vEnemyBarScales[ENEMYBAR_END] = {};

	CTexture*				m_pEnemyBarTex[ENEMYBAR_END] = {};

	class CProgressBar*		m_pHpProgressBar = { nullptr };
	class CProgressBar*		m_pMpProgressBar = { nullptr };

	_int					m_iMaxHp = { 0 };

protected:
	HRESULT Ready_Component();
	HRESULT Render_MainBar();


public:
	static CUI_EnemyBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

