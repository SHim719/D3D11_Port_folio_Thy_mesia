#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_DefaultSkillSlot final : public CUI
{
private:
	CUI_DefaultSkillSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_DefaultSkillSlot(const CUI_DefaultSkillSlot& rhs);
	virtual ~CUI_DefaultSkillSlot() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	HRESULT OnEnter_Layer(void* pArg)	override;
	

public:
	static CUI_DefaultSkillSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

