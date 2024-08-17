#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_PlayerDefault final : public CUI
{
private:
	CUI_PlayerDefault(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PlayerDefault(const CUI_PlayerDefault& rhs);
	virtual ~CUI_PlayerDefault() = default;

	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

public:
	void Update_HpBar(_int iMaxHp, _int iHp);
	void Update_SoulBar(_int iSoulCount);
	void Update_SkillSlot(SKILLTYPE eSkillType);
	void Update_PlunderSlot(SKILLTYPE eSkilType);
	void Update_PotionSlot(_int iPotionCount);

private:
	class CUI_PlayerBar*	m_pPlayerBar	= { nullptr };
	class CUI_SoulBar*		m_pSoulBar		= { nullptr };
	class CUI_SkillSlot*	m_pSkillSlot	= { nullptr };
	class CUI_PlunderSlot*	m_pPlunderSlot	= { nullptr };
	class CUI_PotionSlot*	m_pPotionSlot	= { nullptr };

public:
	static CUI_PlayerDefault* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

