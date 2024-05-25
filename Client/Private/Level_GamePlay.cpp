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
	//CCamera::CAMERADESC camDesc{};
	//camDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	//camDesc.fNear = 0.1f;
	//camDesc.fFar = 300.f;
	//camDesc.fFovy = 60.f;
	//camDesc.vAt = { 0.f, 0.f, 1.f, 1.f };
	//camDesc.vEye = { 0.f, 10.f, 0.f, 1.f };
	//
	//m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Camera", L"Prototype_Free_Camera", &camDesc);
	//
	//
	//_float4 vPosition = _float4(0.f, 0.f, 5.f, 1.f);
	//CGameObject* pPlayer = m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Player", L"Prototype_Player");
	//pPlayer->Get_Transform()->Set_Position(XMLoadFloat4(&vPosition));
	//
	////vPosition = _float4(0.f, 0.f, 5.f, 1.f);
	////CGameObject* pEnemy = m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Enemy", L"Prototype_Enemy");
	////pEnemy->Get_Transform()->Set_Position(XMLoadFloat4(&vPosition));
	//
	//CGameObject* pTestStaticObj = m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Props", L"Prototype_TestStaticObj");
	//pTestStaticObj->Get_Transform()->Set_Position(XMLoadFloat4(&vPosition));
	//
	////vPosition = _float4(3.f, 0.f, 5.f, 1.f);
	////CGameObject* pTestAnim = m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Anim", L"Prototype_TestAnim");
	////pTestAnim->Get_Transform()->Set_Position(XMLoadFloat4(&vPosition));
	//
	// m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Ground", L"Prototype_TestGround");
	//
	// auto wall = m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Ground", L"Prototype_TestGround");
	// wall->Get_Transform()->Set_Scale({ 5.f, 5.f, 1.f });
	// wall->Get_Transform()->Set_Position(XMVectorSet(0.f, 0.f, -5.f, 1.f));

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
