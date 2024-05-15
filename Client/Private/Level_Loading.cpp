#include "Level_Loading.h"
#include "Loader.h"
#include "GameInstance.h"
#include "Levels_Header.h"

#include "Background.h"


CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevel)
{
	m_iLevelID = LEVEL_LOADING;

	m_eNextLevel = eNextLevel;

	m_pGameInstance->Add_Clone(m_iLevelID, L"Background", L"Prototype_Loading_BG");

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Tick(_float TimeDelta)
{
	if (true == m_pLoader->isFinished())
	{
		if (GetKeyState(VK_SPACE) & 0x8000)
		{
			CLevel* pLevel = { nullptr };

			switch (m_eNextLevel)
			{
			//case LEVEL_TITLE:

			case LEVEL_GAMEPLAY:
				pLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
				break;
			}

			if (nullptr == pLevel)
				return;

			if (FAILED(m_pGameInstance->Change_Level(pLevel)))
				return;
		}
	}
}


HRESULT CLevel_Loading::Render()
{
	if (m_pLoader)
		SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());

	return S_OK;
}


CLevel_Loading* CLevel_Loading::Create(LEVEL eNextLevel, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
