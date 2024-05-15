#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() { }

public:
	HRESULT Initialize();

	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	CGameInstance* m_pGameInstance = nullptr;

private:
	HRESULT Open_Level(LEVEL eStartLevelID);

private:
	HRESULT	Ready_Prototype_Component();
	HRESULT Ready_Prototype_Shader();

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END

