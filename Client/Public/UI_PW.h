#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_PW final : public CUI
{
private:
	enum UI_PlagueWeapons { AXE, HAMMER, SPEAR, TWIN, WEAPON_END };

private:
	CUI_PW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PW(const CUI_PW& rhs);
	virtual ~CUI_PW() = default;

	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	HRESULT OnEnter_Layer(void* pArg)	override;

private:
	class CPlayerStats* m_pPlayerStats = { nullptr };

	UI_PlagueWeapons	m_eNowSelected = { AXE };

	_float4				m_vIconPositions[WEAPON_END] = {};
	_float4				m_vUsingIconPosition = {};

	_float3				m_vIconScale = { 112.f, 112.f, 1.f };
	_float3				m_vSelectedScale = { 256.f, 252.f, 1.f };
	_float3				m_vUsingIconScale = { 180.f, 180.f, 1.f };

	_float4x4			m_BGWorldMatrix = {};

	_bool				m_bSkillActived[WEAPON_END] = {};
private:
	CTexture*	m_pUsingSkillTexture = {nullptr};
	CTexture*	m_pInactiveCoverTexture = { nullptr };
	CTexture*	m_pSelectedTexture = { nullptr };
	  
private:
	HRESULT Ready_Component();
	HRESULT Render_BG();
	HRESULT Render_InactiveCover();
	HRESULT Render_Selected();
	HRESULT Render_UsingSkillIcon();

	void Init_UIMatrices();
	void Init_UIPos();
	void Update_SkillActived();
	void Change_UsingSkill();
	void Exit();

	wstring Get_SkillIconTag();

	SKILLTYPE Get_SelectedToSkillType();
public:
	static CUI_PW* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

