#pragma once
#include "State_Base.h"
#include "Player.h"
#include "Main_Camera.h"

BEGIN(Client)
class CPlayerState_Base : public CState_Base
{

protected:
	CPlayerState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerState_Base() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start()				override;
	void OnGoing(_float fTimeDelta)		override;
	void OnState_End()					override;

protected:
	CPlayer*		m_pPlayer = nullptr;
	CMain_Camera*	m_pMain_Camera = nullptr;

public:
	void Free() override;
};

END

