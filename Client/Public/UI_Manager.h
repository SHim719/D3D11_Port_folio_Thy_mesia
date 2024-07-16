#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CUI_Manager
{
	DECLARE_SINGLETON(CUI_Manager)
private:
	CUI_Manager();
	~CUI_Manager();

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	
private:
	CGameInstance*						m_pGameInstance = { nullptr };

	unordered_map<string, class CUI*>	m_UIs;
public:
	void Active_UI(const string& strUITag, void* pArg = nullptr);
	void Inactive_UI(const string& strUITag);

	void Active_Menu();
	void Inactive_Menu();
private:
	HRESULT Ready_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	void Release();
};

END

