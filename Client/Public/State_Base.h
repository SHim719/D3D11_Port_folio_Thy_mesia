#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "GameInstance.h"

BEGIN(Client)

class CState_Base abstract : public CBase
{
protected:
	CState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CState_Base() = default;

public:
	virtual HRESULT Initialize(void* pArg);
	virtual void OnState_Start(void* pArg = nullptr);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual void OnState_End();

	virtual void OnHit(const ATTACKDESC& AttackDesc);

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	CGameInstance*			m_pGameInstance = nullptr;

protected:
	CModel*				m_pModel = nullptr;
	CTransform*			m_pOwnerTransform = nullptr;
	CNavigation*		m_pNavigation = nullptr;

protected:
	_float				m_fPatternTime = { 0.f };
	_float4x4			m_OffsetMatrix = {};

	vector<pair<_uint, ATTACKDESC>>	m_AttackDescs; // ¹«±â ÀÎµ¦½º + ATTACKDESC
	
	size_t				m_iNowAttackIdx = { 0 };
public:
	const pair<_uint, ATTACKDESC>& Get_NowAttackDesc() {
		return m_AttackDescs[m_iNowAttackIdx++];
	}

protected:
	virtual void Init_AttackDesc();

	void Reset_AttackIdx() {
		m_iNowAttackIdx = 0;
	}

	void Setup_RootRotation();

public:
	void Free() override;
};

END

