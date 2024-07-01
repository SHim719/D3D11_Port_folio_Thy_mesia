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

	virtual void Update_PlayerHp(_int iMaxHp, _int iHp);
	virtual void Update_PlayerMp(_int iMaxMp, _int iMp);

	virtual void Update_EnemyHp(_int iHp);
	virtual void Update_EnemyMp(_int iMp);
	virtual void Enemy_FirstHit();
	virtual void Broadcast_Death();
	
	_bool Is_Using() const {
		return m_bUsing;
	}

protected:
	_vector Convert_WorldToScreen(_fvector vUIWorldPos);
	_vector Convert_ScreenToRenderPos(_fvector vUIScreenPos);

protected:
	_float4x4	m_ViewportMatrix;
	_float		m_fAlpha = { 1.f };
	_bool		m_bUsing = { false };

protected:
	CShader*	m_pShader = { nullptr };
	CTexture*	m_pUITexture = { nullptr };
	CVIBuffer*	m_pVIBuffer = { nullptr };

public:
	void Free() override;
};

END

