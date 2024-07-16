#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_PlunderSlot final : public CUI
{
private:
	CUI_PlunderSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PlunderSlot(const CUI_PlunderSlot& rhs);
	virtual ~CUI_PlunderSlot() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	HRESULT OnEnter_Layer(void* pArg)	override;

private:
	class CUI_SkillIcon* m_pSkillIcon = { nullptr };

public:
	void Update_SkillIcon(const SKILLTYPE eSkillType);

public:
	static CUI_PlunderSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

