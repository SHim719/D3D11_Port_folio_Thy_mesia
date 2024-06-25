#include "..\Public\Level_GamePlay.h"
#include "GameInstance.h"

#include "Free_Camera.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	m_iLevelID = LEVEL_GAMEPLAY;

	if (FAILED(Ready_GameObjects()))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{

}

HRESULT CLevel_GamePlay::Render()
{
	SetWindowText(g_hWnd, L"GamePlay");
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_GameObjects()
{

	return S_OK;
}


CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}
