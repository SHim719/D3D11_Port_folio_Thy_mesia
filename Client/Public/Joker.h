#pragma once

#include "Enemy.h"
#include "Joker_Enums.h"

BEGIN(Client)

class CJoker final : public CEnemy
{
public:
	enum JOKER_WEAPON { HAMMER, WEAPON_END };

private:
	CJoker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CJoker(const CJoker& rhs);
	virtual ~CJoker() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;

	void Bind_KeyFrames()				override;
	void Bind_KeyFrameSounds()			override;

	void Percept_Target()				override;
	void Change_To_NextComboAnim();

	void StrongAttack_Impact();

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Ready_States();
	HRESULT Ready_Weapon();
	HRESULT Ready_Stats();
	HRESULT Ready_UI();

public:
	static CJoker* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	void Free() override;
};

END