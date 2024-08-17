#pragma once

#include "Client_Defines.h"
#include "Weapon.h"
#include "GameEffect.h"


BEGIN(Client)

class CUrd_Weapon : public CWeapon
{
private:
	CUrd_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUrd_Weapon(const CUrd_Weapon& rhs);
	virtual ~CUrd_Weapon() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	HRESULT Render()					override;

	HRESULT OnEnter_Layer(void* pArg)	override;
private:
	_bool	m_bReleased = { false };
	_bool	m_bLanded = { false };
	_bool	m_bThrow = { false };
	_bool	m_bParried = { false };

	_bool	m_bUsing = { false };

private:
	ATTACKDESC m_LandingImpactAtkDesc = {};

public:
	void Released(_bool bThrow);
	
	_bool Is_Using() const {
		return m_bUsing;
	}

	void Active_MagicCircle(_bool bUltimate);
	void Explode_MagicCircle();


	void Active_LandingImpact();

	CGameEffect::EFFECTSPAWNDESC	m_EffectSpawnDesc = {};
	
	class CEffect_Group*			m_pEffect_SwordDefault = { nullptr };
	class CUrd_MagicCircle*			m_pMagicCircle = { nullptr };
private:
	void OnCollisionEnter(CGameObject* pOther)	override;

private:
	HRESULT Ready_Components(WEAPONDESC* pDesc);
	void Init_AttackDesc();

public:
	static CUrd_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END