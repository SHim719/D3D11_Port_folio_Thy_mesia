#pragma once

#include "Client_Defines.h"
#include "Weapon.h"


BEGIN(Client)

class COdur_Card final : public CWeapon
{
private:
	COdur_Card(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COdur_Card(const COdur_Card& rhs);
	virtual ~COdur_Card() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	HRESULT OnEnter_Layer(void* pArg)	override;
private:
	_float		m_fLifeSpan = { 5.f };
	_float		m_fTimeAcc = { 0.f };


private:
	void OnCollisionEnter(CGameObject* pOther)	override;

private:
	CShader*	m_pShader = { nullptr };
	CModel*		m_pModel = { nullptr };
	CCollider*	m_pCollider = { nullptr };

private:
	HRESULT Ready_Component();

public:
	static COdur_Card* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	void Free()	override;
};

END




