#include "Level_Tool.h"
#include "GameInstance.h"

#include "ToolMapObj.h"
#include "ToolAnimObj.h"
#include "TestGround.h"
#include "Free_Camera.h"
#include "Main_Camera.h"
#include "Player.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tool::Initialize()
{
	m_iLevelID = LEVEL_TOOL;

	m_pGameInstance->Add_Prototype(L"Prototype_ToolMapObj", CToolMapObj::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(L"Prototype_ToolAnimObj", CToolAnimObj::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(L"Prototype_Free_Camera", CFree_Camera::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(L"Prototype_Main_Camera", CMain_Camera::Create(m_pDevice, m_pContext));
	
	m_pGameInstance->Add_Prototype(LEVEL_TOOL, L"Prototype_BG_Texture", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Thymesia.jpg"));
	m_pGameInstance->Add_Prototype(L"Prototype_TestGround", CTestGround::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Clone(LEVEL_TOOL, L"TestGround", L"Prototype_TestGround");

	m_pGameInstance->Add_Prototype(LEVEL_TOOL, L"Prototype_Model_Player", CModel::Create(m_pDevice, m_pContext, 
		"../../Resources/Models/Corvus/", "Corvus_No1.dat"));
	m_pGameInstance->Add_Prototype(L"Prototype_Player", CPlayer::Create(m_pDevice, m_pContext));

	CCamera::CAMERADESC camDesc{};
	camDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	camDesc.fNear = 0.1f;
	camDesc.fFar = 300.f;
	camDesc.fFovy = 60.f;
	camDesc.vAt = { 0.f, 0.f, 1.f, 1.f };
	camDesc.vEye = { 0.f, 10.f, 0.f, 1.f };

	m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Camera", L"Prototype_Free_Camera", &camDesc);
	return S_OK;
}

void CLevel_Tool::Tick(_float fTimeDelta)
{
	
}

HRESULT CLevel_Tool::Render()
{
	
	return S_OK;
}

CLevel_Tool* CLevel_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Tool* pInstance = new CLevel_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Tool::Free()
{
	__super::Free();


}