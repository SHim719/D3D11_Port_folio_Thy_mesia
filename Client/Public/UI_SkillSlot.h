#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_SkillSlot final : public CUI
{
private:
	CUI_SkillSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_SkillSlot(const CUI_SkillSlot& rhs);
	virtual ~CUI_SkillSlot() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;
	HRESULT Render_CoolDown();

	HRESULT OnEnter_Layer(void* pArg)	override;

	void Set_CoolDownRatio(_float fRatio) {
		m_fCoolDownRatio = fRatio;
	}
	
private:
	class CUI_SkillIcon*	m_pSkillIcon = { nullptr };
	CTexture*				m_pCoolDown_Texture = { nullptr };

private:
	_float					m_fCoolDownRatio = { 1.f };

public:
	void Update_SkillIcon(const SKILLTYPE eSkillType);
	
public:
	static CUI_SkillSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

