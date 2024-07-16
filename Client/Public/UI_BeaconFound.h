#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_BeaconFound final : public CUI
{
private:
	enum MessageState { Appear, Idle, Disappear };

private:
	CUI_BeaconFound(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BeaconFound(const CUI_BeaconFound& rhs);
	virtual ~CUI_BeaconFound() = default;

	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	HRESULT OnEnter_Layer(void* pArg)	override;

	HRESULT Render_BG();
	HRESULT Render_Message();

private:
	MessageState	m_eState = { Appear };

	_float3			m_vBGScale = {};
	_float3			m_vMessageScale = {};
					
	_float			m_fScalingSpeed = { 0.1f };
	_float			m_fNowScaling = { 1.f };
					
	_float			m_fDeltaAlphaSpeed = { 1.f };
					
	_float			m_fDuration = { 1.5f };
	_float			m_fTimeAcc = { 0.f };

private:
	CTexture*		m_pBGTexture = { nullptr };

private:
	HRESULT Ready_Component();

public:
	static CUI_BeaconFound* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

