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
	void Bind_KeyFrames()	override;

private:
	CShader*	m_pShader = { nullptr };
	CModel*		m_pModel = { nullptr };
	CCollider*	m_pCollider = { nullptr };

private:
	PlayerState			m_eState = { PlayerState::State_End };
	class CState_Base*	m_States[(_uint)PlayerState::State_End] = {};

	class CPlayerStat*	m_pStats = { nullptr };
public:
	void Change_State(_uint eState) override;

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
	CTransform*		m_pTargetTransform = { nullptr };
	_bool			m_bLockOn = { false };
	_bool			m_bCanParry = { true };

public:
	void Toggle_LockOn(CTransform* pTargetTransform = nullptr);
	_bool Is_LockOn() const { return m_bLockOn; }
	void Enable_Parry() { m_bCanParry = true; }
	void Disable_Parry() { m_bCanParry = false; }

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