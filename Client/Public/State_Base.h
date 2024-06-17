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
	virtual void OnGoing(_float fTimeDelta);
	virtual void OnState_End();

	virtual void OnHit(void* pArg);

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	CGameInstance*			m_pGameInstance = nullptr;

protected:
	CModel*				m_pModel = nullptr;
	CTransform*			m_pOwnerTransform = nullptr;

protected:
	_float				m_fPatternTime = { 0.f };

	vector<ATTACKDESC>	m_AttackDescs;

public:
	const ATTACKDESC& Get_NowAttackDesc(size_t iIdx) const {
		return m_AttackDescs[iIdx];
	}

protected:
	virtual void Init_AttackDesc();

public:
	void Free() override;
};

END

