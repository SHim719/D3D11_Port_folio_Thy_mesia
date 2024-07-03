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
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	void Bind_KeyFrames()				override;

private:
	_float	m_fRotRate = { 6.f };

private:
	void Percept_Target()						override;
	void SetState_Executed(void* pArg)			override;
	void SetState_Death()						override;

	void Change_To_NextComboAnim();

	_bool Is_Death() override {
		return m_iState == (_uint)JokerState::State_Death;
	}

	_bool Is_Stunned() override {
		return m_iState == (_uint)JokerState::State_Stunned_Loop;
	}
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