#pragma once

#include "Enemy.h"
#include "VillagerM_Enums.h"

BEGIN(Client)

class CVillager_M final : public CEnemy
{
public:
	enum VILLIGER_M_WEAPON { AXE, WEAPON_END };

private:
	CVillager_M(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVillager_M(const CVillager_M& rhs);
	virtual ~CVillager_M() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;

	void Bind_KeyFrames()				override;
	void Percept_Target()				override;

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Ready_States();
	HRESULT Ready_Weapon();
	HRESULT Ready_Stats();
	HRESULT Ready_UI();

public:
	static CVillager_M* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	void Free() override;
};

END