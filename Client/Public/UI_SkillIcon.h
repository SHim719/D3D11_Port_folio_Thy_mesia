#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_SkillIcon final : public CUI
{
private:
	CUI_SkillIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_SkillIcon(const CUI_SkillIcon& rhs);
	virtual ~CUI_SkillIcon() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	HRESULT OnEnter_Layer(void* pArg)	override;

public:
	void Change_Texture(const SKILLTYPE eSkillType);

public:
	static CUI_SkillIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

