#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CPerceptionBounding final : public CGameObject
{
private:
	CPerceptionBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPerceptionBounding(const CPerceptionBounding& rhs);
	virtual ~CPerceptionBounding() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	class CEnemy*	m_pOwner = { nullptr };
	CCollider*		m_pPerceptionCollider = { nullptr };

private:
	void OnCollisionEnter(CGameObject* pOther)	override;

private:
	HRESULT Ready_Component();

public:
	static CPerceptionBounding* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	void Free() override;
};

END