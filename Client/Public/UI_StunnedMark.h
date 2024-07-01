#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_StunnedMark final : public CUI
{
private:
	CUI_StunnedMark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_StunnedMark(const CUI_StunnedMark& rhs);
	virtual ~CUI_StunnedMark() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	HRESULT OnEnter_Layer(void* pArg)	override;

private:
	ATTACHDESC m_AttachDesc = {};

public:
	static CUI_StunnedMark* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

