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

	class CPlayer*						m_pPlayer = { nullptr };

	unordered_map<string, pair<size_t, vector<class CUI*>>>	m_UIs;
public:
	size_t Active_UI(const string& strUITag, void* pArg = nullptr);
	void InActive_UI(const string& strUITag, size_t iIdx = 0);

	void Set_Player(CPlayer* pPlayer) {
		m_pPlayer = pPlayer;
	}
private:
	HRESULT Ready_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	void Release();
};

END

