#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CEventTrigger final : public CGameObject
{
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
	CShader*	m_pShader = { nullptr };
	CCollider*	m_pCollider = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CEventTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END

