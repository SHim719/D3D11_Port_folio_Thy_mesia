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

	virtual void Update_PlayerHp(_float fMaxHp, _float fHp);
	virtual void Update_PlayerMp(_int iMaxMp, _int iMp);

protected:
	_vector Convert_WorldToScreen(_fvector vUIWorldPos);
	_vector Convert_ScreenToWorld(_fvector vUIScreenPos);

protected:
	_float4x4	m_ViewportMatrix;


protected:
	CShader*	m_pShader = { nullptr };
	CTexture*	m_pUITexture = { nullptr };
	CVIBuffer*	m_pVIBuffer = { nullptr };

public:
	void Free() override;
};

END

