#pragma once

#include "UI.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class CUI_EnemyBar final : public CUI
{
public:
	typedef struct tagUIEnemyBarDesc
	{
		CTransform* pOwnerTransform = nullptr;
		_float4		vOffset = {};
		class CEnemyStats* pStats = nullptr;
	}UIENEMYBARDESC;

private:
	CUI_EnemyBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_EnemyBar(const CUI_EnemyBar& rhs);
	virtual ~CUI_EnemyBar() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	HRESULT Render_MainBar();
	HRESULT Render_StunnedShine();

public:
	void Update_EnemyHp(_int iHp); 
	void Update_EnemyMp(_int iMp);

private:
	_float4					m_vRenderStartPos = {};
	_bool					m_bStunned = { false };

	CTexture*				m_pStunnedShineTex = { nullptr };

	_int					m_iMaxHp = { 0 };

	class CProgressBar*		m_pHpProgressBar = { nullptr };
	class CProgressBar*		m_pMpProgressBar = { nullptr };

	CTransform*				m_pOwnerTransform = { nullptr };
	_float4					m_vOffset = {};

	_float					m_fDeltaAlphaSpeed = { -2.f };

private:
	HRESULT Ready_Component();

public:
	static CUI_EnemyBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

