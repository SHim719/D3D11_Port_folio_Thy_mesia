#pragma once

#include "Enemy.h"
#include "TwinBladeKnight_Enums.h"

BEGIN(Client)

class CTwinBladeKnight final : public CEnemy
{
public:
	enum TWINBLADEKNIGHT_WEAPON { BLADE_L, BLADE_R, WEAPON_END };

private:
	CTwinBladeKnight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTwinBladeKnight(const CTwinBladeKnight& rhs);
	virtual ~CTwinBladeKnight() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	void Bind_KeyFrames()				override;

private:
	_float	m_fRotRate = { 10.f };

private:
	void Percept_Target()						override;
	void SetState_Executed(void* pArg)			override;
	void SetState_Death()						override;

	void Change_To_NextComboAnim();
	void Active_Weapons();

	_bool Is_Death() override {
		return m_iState == (_uint)TwinBladeKnight_State::State_Death;
	}
	
	_bool Is_Stunned() override {
		return m_iState == (_uint)TwinBladeKnight_State::State_Stunned_Loop;
	}

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Ready_States();
	HRESULT Ready_Weapon();
	HRESULT Ready_Stats();
	HRESULT Ready_UI();

public:
	static CTwinBladeKnight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	void Free() override;
};

END