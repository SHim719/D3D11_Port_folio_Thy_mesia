#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_NeedKey : public CUI
{
private:
	CUI_NeedKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_NeedKey(const CUI_NeedKey& rhs);
	virtual ~CUI_NeedKey() = default;

private:
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	_float m_fDuration = { 2.f };


private:
	HRESULT Ready_Component();

public:
	static CUI_NeedKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

