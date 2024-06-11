#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Player_Enums.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	CShader*	m_pShader = { nullptr };
	CModel*		m_pModel = { nullptr };

private:
	PlayerState			m_eState = { PlayerState::State_End };
	class CState_Base*	m_States[(_uint)PlayerState::State_End] = {};

	class CPlayerStat*	m_pStats = { nullptr };
public:
	void Change_State(PlayerState eState);

	CPlayerStat* Get_PlayerStat() const { 
		return m_pStats; }

private:
	class CWeapon*	m_pDagger = { nullptr };
	class CWeapon*	m_pSaber = { nullptr };

private:
	void Active_Weapons();
	void InActive_Weapons();
	void Active_Claw();
	void InActive_Claw();

private:
	HRESULT Ready_Components();
	HRESULT Ready_States();
	HRESULT Ready_Weapons();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END