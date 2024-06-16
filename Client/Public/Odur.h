#pragma once

#include "Enemy.h"
#include "Odur_Enums.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class COdur final : public CEnemy
{
private:
	enum ODUR_WEAPON { CANE, SWORD, FOOT_L, FOOT_R, WEAPON_END};

private:
	COdur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COdur(const COdur& rhs);
	virtual ~COdur() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	void Bind_KeyFrames()				override;

private:
	CBone*	m_pSwapBone = { nullptr };
	_float	m_fRotRate = { 15.f };

	_float	m_fAlpha = { 1.f };
	_float	m_fDeltaAlphaSpeed = { 2.f };
	_bool	m_bAlphaEnable = { false };
	_bool	m_bAlphaIncrease = { false };

public:
	void Swap_Bone();

	void Set_Alpha_Increase() {
		m_bAlphaIncrease = true; m_bAlphaEnable = true;
	}

	void Set_Alpha_Decrease() {
		m_bAlphaIncrease = false; m_bAlphaEnable = true;
	}

private:
	void Update_Alpha();

private:
	void OnCollisionEnter(CGameObject* pOther)	override;
	void OnCollisionExit(CGameObject* pOther)	override;

private:
	HRESULT Ready_Components();
	HRESULT Ready_States();
	HRESULT Ready_Weapons();

public:
	static COdur* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END