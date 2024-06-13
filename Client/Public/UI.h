#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CUI abstract : public CGameObject
{
protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	virtual void On_UIActive(void* pArg);

protected:
	_vector Convert_WorldToScreen(_fvector vUIScreenPos);
	_vector Convert_ScreenToWorld(_fvector vUIWorldPos);

protected:
	_float4x4 m_ViewportMatrix;


protected:
	CShader*	m_pShader = { nullptr };
	CTexture*	m_pUITexture = { nullptr };
	CVIBuffer*	m_pVIBuffer = { nullptr };

public:
	void Free() override;
};

END

