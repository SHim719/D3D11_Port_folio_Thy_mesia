#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "GameInstance.h"

#include "GameObject.h"
#include "Free_Camera.h"


BEGIN(Client)

class CToolState : public CBase
{
protected:
	CToolState(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CToolState() = default;

public:
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	class CGameInstance*	m_pGameInstance = { nullptr };

	class CFree_Camera*		m_pCamera = nullptr;
public:
	virtual void Free() override;
};

END

