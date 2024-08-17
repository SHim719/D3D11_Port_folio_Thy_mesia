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

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	return S_OK;
}

void CLevel_Loading::Tick(_float TimeDelta)
{
	if (true == m_pLoader->isFinished())
	{
		if (GetKeyState(VK_RETURN) & 0x8000)
		{
			CLevel* pLevel = { nullptr };

			switch (m_eNextLevel)
			{
			case LEVEL_STAGE1:
				pLevel = CLevel_Stage1::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_ODUR:
				pLevel = CLevel_Odur::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_URD:
				pLevel = CLevel_Urd::Create(m_pDevice, m_pContext);
				break;
			}

			if (nullptr == pLevel)
				return;

			ADD_EVENT(bind(&CGameInstance::Change_Level, m_pGameInstance, pLevel));
			   
		}
	}
}


HRESULT CLevel_Loading::Render()
{
	if (m_pLoader)
		m_pGameInstance->Render_Font(L"Main_Font", m_pLoader->Get_LoadingText(), _float2(0.f, 0.f));

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
