#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CEventTrigger final : public CGameObject
{
private:
	enum TRIGGEREVENTS { START_ODUR_CUTSCENE, START_URD_CUTSCENE, EVENT_END };

private:
	CEventTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEventTrigger(const CEventTrigger& rhs);
	virtual ~CEventTrigger() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	TRIGGEREVENTS	m_eTriggerEvent = EVENT_END;

	_bool			m_bNextFrameDestroy = { false };

private:
	void OnCollisionEnter(CGameObject* pOther)	override;

private:
	CCollider*		m_pCollider = { nullptr };

private:
	HRESULT Ready_Components(void* pArg);

public:
	static CEventTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END

