#include "UI_Manager.h"
#include "UI_Headers.h"

#include "GameInstance.h"

#include "Player.h"

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
	CShader* pShader = static_cast<CShader*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, L"Prototype_Shader_UI"));

	_float4x4 ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f)));

	pShader->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	pShader->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	Safe_Release(pShader);

	if (FAILED(Ready_UI(pDevice, pContext)))
		return E_FAIL;

	return S_OK;
}

CUI* CUI_Manager::Active_UI(const string& strUITag, void* pArg)
{
	auto it = m_UIs.find(strUITag);

	if (m_UIs.end() == it)
		return nullptr;

	it->second->Add_PoolingObject_To_Layer(L"UI", nullptr);
	it->second->On_UIActive(pArg);

	return it->second;
}

void CUI_Manager::InActive_UI(const string& strUITag)
{
	auto it = m_UIs.find(strUITag);

	if (m_UIs.end() == it)
		return;

	it->second->Set_ReturnToPool(true);
}

HRESULT CUI_Manager::Ready_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI* pUI = CUI_LockOn::Create(pDevice, pContext);
	if (FAILED(pUI->Initialize(nullptr)))
		return E_FAIL;
	
	m_UIs.emplace("UI_LockOn", pUI);

	pUI = CUI_PlayerBar::Create(pDevice, pContext);
	if (FAILED(pUI->Initialize(m_pPlayer->Get_PlayerStat())))
		return E_FAIL;

	m_UIs.emplace("UI_PlayerBar", pUI);

	return S_OK;
}

void CUI_Manager::Release()
{
	for (auto& Pair : m_UIs)
		Safe_Release(Pair.second);

	m_UIs.clear();
}
