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

size_t CUI_Manager::Active_UI(const string& strUITag, void* pArg)
{
	auto it = m_UIs.find(strUITag);

	if (m_UIs.end() == it)
		return ULLONG_MAX;

	pair<size_t, vector<CUI*>>& UIPool = it->second;

	size_t iUIIdx = UIPool.first;

	CUI* pActiveUI = UIPool.second[UIPool.first++];
	UIPool.first = UIPool.first >= UIPool.second.size() ? 0 : UIPool.first;

	pActiveUI->OnEnter_Layer(pArg);

	Safe_AddRef(pActiveUI);

	ADD_EVENT(bind(&CGameInstance::Insert_GameObject, m_pGameInstance, GET_CURLEVEL, L"UI", pActiveUI));

	return iUIIdx;
}

void CUI_Manager::InActive_UI(const string& strUITag, size_t iIdx)
{
	auto it = m_UIs.find(strUITag);

	if (m_UIs.end() == it)
		return;

	pair<size_t, vector<CUI*>>& UIPool = it->second;

	CUI* pDestUI = UIPool.second[iIdx];

	pDestUI->Set_ReturnToPool(true);
}

HRESULT CUI_Manager::Ready_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI* pUI = CUI_LockOn::Create(pDevice, pContext);
	if (FAILED(pUI->Initialize(nullptr)))
		return E_FAIL;
	
	m_UIs.emplace("UI_LockOn", (make_pair(0, vector<CUI*>{pUI})));

	pUI = CUI_PlayerBar::Create(pDevice, pContext);
	if (FAILED(pUI->Initialize(m_pPlayer->Get_PlayerStats())))
		return E_FAIL;

	m_UIs.emplace("UI_PlayerBar", make_pair(0, vector<CUI*>{pUI}));

	pUI = CFadeScreen::Create(pDevice, pContext);
	if (FAILED(pUI->Initialize(nullptr)))
		return E_FAIL;

	m_UIs.emplace("FadeScreen", make_pair(0, vector<CUI*>{pUI}));

	pUI = CUI_BossBar::Create(pDevice, pContext);
	if (FAILED(pUI->Initialize(nullptr)))
		return E_FAIL;

	m_UIs.emplace("UI_BossBar", make_pair(0, vector<CUI*>{pUI}));

	vector<CUI*> DamageFonts;
	DamageFonts.reserve(5);
	for (_int i = 0; i < 5; ++i)
	{
		pUI = CUI_DamageFont::Create(pDevice, pContext);
		if (FAILED(pUI->Initialize(nullptr)))
			return E_FAIL;
		DamageFonts.emplace_back(pUI);
	}

	m_UIs.emplace("UI_DamageFont", make_pair(0, DamageFonts));

	vector<CUI*> EnemyBars;
	EnemyBars.reserve(5);
	for (_int i = 0; i < 5; ++i)
	{
		pUI = CUI_EnemyBar::Create(pDevice, pContext);
		if (FAILED(pUI->Initialize(nullptr)))
			return E_FAIL;
		EnemyBars.emplace_back(pUI);
	}

	m_UIs.emplace("UI_EnemyBar", make_pair(0, EnemyBars));

	vector<CUI*> StunnedMarks;
	StunnedMarks.reserve(5);
	for (_int i = 0; i < 5; ++i)
	{
		pUI = CUI_StunnedMark::Create(pDevice, pContext);
		if (FAILED(pUI->Initialize(nullptr)))
			return E_FAIL;
		StunnedMarks.emplace_back(pUI);
	}

	m_UIs.emplace("UI_StunnedMark", make_pair(0, StunnedMarks));

	pUI = CUI_PlunderSlot::Create(pDevice, pContext);
	if (FAILED(pUI->Initialize(m_pPlayer->Get_PlayerStats())))
		return E_FAIL;

	m_UIs.emplace("UI_PlunderSlot", make_pair(0, vector<CUI*>{pUI}));

	return S_OK;
}

void CUI_Manager::Release()
{
	for (auto& Pair : m_UIs)
		for (auto& pUI : Pair.second.second)
			Safe_Release(pUI);

	m_UIs.clear();
}
