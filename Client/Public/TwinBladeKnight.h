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

	void Bind_KeyFrames()				override;
	void Bind_KeyFrameSounds()			override;

	void Percept_Target()				override;

	void Change_To_NextComboAnim();
	void Active_Weapons();

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