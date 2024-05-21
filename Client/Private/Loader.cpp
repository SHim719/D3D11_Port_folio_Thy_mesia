#include "Loader.h"

#include "GameInstance.h"

#include "Free_Camera.h"
#include "Player.h"
#include "Enemy.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

unsigned int APIENTRY Loading_Main(void* pArg)
{
	CLoader* pLoader = (CLoader*)pArg;

	return pLoader->Loading();
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Loading_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

unsigned int CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT hr = S_OK;

	switch (m_eNextLevelID)
	{
	case LEVEL_TITLE:
	case LEVEL_GAMEPLAY:
		hr = Load_Default();
		break;
	case LEVEL_TOOL:
		hr = Loading_ToolLevel();
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return 1;

	return 0;

}

HRESULT CLoader::Load_Default()
{
	lstrcpy(m_szLoadingText, TEXT("Load Model."));


	_matrix		PivotMatrix = XMMatrixIdentity();

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(To_Radian(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Player_Body"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Resources/Models/Player/", "PlayerAnim.fbx", { 1, 2 }, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(To_Radian(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Player_Head"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_SKELETALMESH, "../../Resources/Models/Player/", "Player_Head.fbx", {}, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(To_Radian(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Model_PuppetString"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_SKELETALMESH, "../../Resources/Models/Player/", "PuppetString.fbx", {}, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Model_BlackBoard"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Models/Props/", "BlackBoard.fbx", {}, PivotMatrix))))
		return E_FAIL;

	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(To_Radian(90.f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Model_TestAnim"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Resources/Models/AnimTest/", "Anim.fbx", PivotMatrix))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Load Prototype."));

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_Free_Camera"), CFree_Camera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_Player"), CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_Enemy"), CEnemy::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("Load Complete."));

	m_bIsFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_ToolLevel()
{
	lstrcpy(m_szLoadingText, TEXT("Load Complete."));
	m_bIsFinished = true;
	return S_OK;
}


CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
