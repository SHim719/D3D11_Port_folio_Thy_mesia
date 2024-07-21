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

void CUI_Manager::Active_UI(const string& strUITag, void* pArg)
{
	auto it = m_UIs.find(strUITag);

	if (m_UIs.end() == it)
		return;

	CUI* pActiveUI = it->second;

	pActiveUI->OnEnter_Layer(pArg);

	Safe_AddRef(pActiveUI);

	ADD_EVENT(bind(&CGameInstance::Insert_GameObject, m_pGameInstance, GET_CURLEVEL, L"UI", pActiveUI));
}


void CUI_Manager::Inactive_UI(const string& strUITag)
{
	auto it = m_UIs.find(strUITag);

	if (m_UIs.end() == it)
		return;

	it->second->Set_ReturnToPool(true);
}

void CUI_Manager::Active_Menu()
{
	Inactive_UI("UI_PlayerDefault");

	Active_UI("UI_Menu");

	//ShowCursor(TRUE);
}

void CUI_Manager::Inactive_Menu()
{
	Active_UI("UI_PlayerDefault");

	Inactive_UI("UI_Menu");

	GET_PLAYER->ChangeAnim_GetUp();

	CFadeScreen::FADEDESC FadeDesc = {};
	FadeDesc.eFadeColor = CFadeScreen::BLACK;
	FadeDesc.fFadeOutSpeed = 1.f;
	FadeDesc.fFadeInSpeed = 1.f;

	UIMGR->Active_UI("FadeScreen", &FadeDesc);

	//ShowCursor(FALSE);
}

HRESULT CUI_Manager::Ready_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI* pUI = CUI_LockOn::Create(pDevice, pContext);
	if (FAILED(pUI->Initialize(nullptr)))
		return E_FAIL;
	
	m_UIs.emplace("UI_LockOn", pUI);

	pUI = CUI_PlayerDefault::Create(pDevice, pContext);

	m_UIs.emplace("UI_PlayerDefault", pUI);

	pUI = CFadeScreen::Create(pDevice, pContext);
	if (FAILED(pUI->Initialize(nullptr)))
		return E_FAIL;

	m_UIs.emplace("FadeScreen", pUI);

	pUI = CUI_BossBar::Create(pDevice, pContext);
	if (FAILED(pUI->Initialize(nullptr)))
		return E_FAIL;

	m_UIs.emplace("UI_BossBar", pUI);

	pUI = CUI_StunnedMark::Create(pDevice, pContext);
	if (FAILED(pUI->Initialize(nullptr)))
		return E_FAIL;

	m_UIs.emplace("UI_StunnedMark", pUI);

	pUI = CUI_BeaconFound::Create(pDevice, pContext);
	if (FAILED(pUI->Initialize(nullptr)))
		return E_FAIL;

	m_UIs.emplace("UI_BeaconFound", pUI);

	pUI = CUI_Menu::Create(pDevice, pContext);
	if (FAILED(pUI->Initialize(nullptr)))
		return E_FAIL;

	m_UIs.emplace("UI_Menu", pUI);

	pUI = CUI_Stats::Create(pDevice, pContext);
	m_UIs.emplace("UI_Stats", pUI);

	pUI = CUI_PW::Create(pDevice, pContext);
	m_UIs.emplace("UI_PW", pUI);

	pUI = CUI_Popup::Create(pDevice, pContext);
	m_UIs.emplace("UI_Popup", pUI);

	return S_OK;
}

void CUI_Manager::Release()
{
	for (auto& Pair : m_UIs)
		Safe_Release(Pair.second);

	m_UIs.clear();
}
