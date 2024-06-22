#pragma once

#include "Enemy.h"
#include "Odur_Enums.h"
#include "Cutscene_Actor.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class COdur final : public CEnemy, public CCutscene_Actor
{
private:
	enum ODUR_WEAPON { CANE, SWORD, FOOT_L, FOOT_R, WEAPON_END};

private:
	COdur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COdur(const COdur& rhs);
	virtual ~COdur() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	void OnEnter_Cutscene()									override;
	void OnStart_Cutscene(CUTSCENE_NUMBER eCutsceneNumber)	override;
	void OnEnd_Cutscene()									override;

	void Bind_KeyFrames()				override;

private:
	CBone*	m_pSwapBone = { nullptr };
	_float	m_fRotRate = { 8.f };

	_float	m_fAlpha = { 1.f };
	_float	m_fDeltaAlphaSpeed = { 2.f };
	_bool	m_bAlphaEnable = { false };
	_bool	m_bAlphaIncrease = { false };

	_bool  m_bCardPattern = { false };
public:
	void Swap_Bone();

	void Set_Alpha_Increase() {
		m_bAlphaIncrease = true; m_bAlphaEnable = true;
	}

	void Set_Alpha_Decrease() {
		m_bAlphaIncrease = false; m_bAlphaEnable = true;
	}

	void Set_Alpha(_float fAlpha) {
		m_fAlpha = fAlpha; Update_WeaponAlpha();
	}

	_bool Is_CardPattern() const {
		return m_bCardPattern;
	}

	void Set_CardPattern(_bool bCardPattern) {
		m_bCardPattern = m_bCardPattern;
	}

private:
	void Update_Alpha(_float fTimeDelta);
	void Update_WeaponAlpha();


private:
	void OnCollisionEnter(CGameObject* pOther)	override;
	void OnCollisionExit(CGameObject* pOther)	override;

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Ready_States();
	HRESULT Ready_Weapons();

public:
	static COdur* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END