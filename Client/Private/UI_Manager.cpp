#include "UI_Manager.h"
#include "UI_Headers.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

CUI_Manager::~CUI_Manager()
{
	Safe_Release(m_pGameInstance);

	Release();
}

HRESULT CUI_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (FAILED(Ready_UI(pDevice, pContext)))
		return E_FAIL;


	return S_OK;
}

CUI* CUI_Manager::Active_UI(const string& strUITag, void* pArg)
{
	auto it = m_UIs.find(strUITag);

	if (m_UIs.end() == it)
		return nullptr;

	m_pGameInstance->Insert_GameObject(LEVEL_STATIC, L"UI", it->second);

	it->second->On_UIActive(pArg);

	Safe_AddRef(it->second);

	return it->second;
}

void CUI_Manager::InActive_UI(const string& strUITag)
{
	auto it = m_UIs.find(strUITag);

	if (m_UIs.end() == it)
		return;

	it->second->Set_Destroy(true);

	return;
}

HRESULT CUI_Manager::Ready_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI* pUI = CUI_LockOn::Create(pDevice, pContext);
	if (FAILED(pUI->Initialize(nullptr)))
		return E_FAIL;


	m_UIs.emplace("UI_LockOn", pUI);
	return S_OK;
}

void CUI_Manager::Release()
{
	for (auto& Pair : m_UIs)
		Safe_Release(Pair.second);

	m_UIs.clear();
}
