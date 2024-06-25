#pragma once

#include "UI_EnemyBar.h"

BEGIN(Client)

class CUI_BossBar final : public CUI_EnemyBar
{
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

	HRESULT OnEnter_Layer(void* pArg)	override;
private:
	void Draw_BossNameText();

private:
	wstring m_wstrBossName = L"";

public:
	static CUI_BossBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

