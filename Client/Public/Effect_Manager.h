#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"
#include "Effect_Group.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CEffect_Manager
{
	DECLARE_SINGLETON(CEffect_Manager)

private:
	CEffect_Manager();
	~CEffect_Manager();

public:
	HRESULT Initialize();

	void Active_Effect(const string& strTag, void* pArg);
	void Inactive_Effect(const string& strTag, size_t iIdx = 0);

	void Create_Effect_Hit(const string& strTag, void* pArg);

	CEffect_Group* Get_EffectGroup(const string& strTag);
private:
	HRESULT Init_Effects();
	HRESULT Add_EffectGroups(const string& strEffectGroupTags, _uint iNumGroups, const wstring& wstrPrototypeTag);

private:
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	unordered_map<string, vector<CEffect_Group*>> m_EffectGroups;

private:
	void Release();
};

END


