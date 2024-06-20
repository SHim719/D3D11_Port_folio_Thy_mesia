#include "Level_Tool.h"
#include "GameInstance.h"

#include "ToolMapObj.h"
#include "ToolAnimObj.h"
#include "ToolColliderObj.h"
#include "ToolNaviCell.h"
#include "ToolNaviCellPoint.h"

#include "TestGround.h"
#include "Free_Camera.h"
#include "Main_Camera.h"
#include "Player.h"
#include "Weapon.h"

#include "Odur.h"
#include "Odur_Card.h"

#include "UI_Headers.h"
#include "UI_Manager.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tool::Initialize()
{
	m_iLevelID = LEVEL_TOOL;

	if (FAILED(Ready_ToolObjects()))
		return E_FAIL;

	m_pGameInstance->Add_Prototype(LEVEL_TOOL, L"Prototype_BG_Texture", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Thymesia.jpg"));
	m_pGameInstance->Add_Prototype(L"Prototype_TestGround", CTestGround::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(L"Prototype_Weapon", CWeapon::Create(m_pDevice, m_pContext));

	if (FAILED(Ready_Navigation()))
		return E_FAIL;
	
	if (FAILED(Ready_Camera()))
		return E_FAIL;

	if (FAILED(Ready_Player()))
		return E_FAIL;

	//if (FAILED(Ready_Odur()))
	//	return E_FAIL;

	//if (FAILED(Ready_UIResource()))
	//	return E_FAIL;
	//
	//if (FAILED(Ready_UI()))
	//	return E_FAIL;

	UIMGR->Initialize(m_pDevice, m_pContext);

	return S_OK;
}

void CLevel_Tool::Tick(_float fTimeDelta)
{
	
}

HRESULT CLevel_Tool::Render()
{
	
	return S_OK;
}

HRESULT CLevel_Tool::Ready_ToolObjects()
{
	m_pGameInstance->Add_Prototype(L"Prototype_ToolMapObj", CToolMapObj::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(L"Prototype_ToolAnimObj", CToolAnimObj::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(L"Prototype_ToolColliderObj", CToolColliderObj::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(L"Prototype_ToolNaviCell", CToolNaviCell::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(L"Prototype_ToolNaviCellPoint", CToolNaviCellPoint::Create(m_pDevice, m_pContext));

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Camera()
{
	m_pGameInstance->Add_Prototype(L"Prototype_Free_Camera", CFree_Camera::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(L"Prototype_Main_Camera", CMain_Camera::Create(m_pDevice, m_pContext));

	CCamera::CAMERADESC camDesc{};
	camDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	camDesc.fNear = 0.1f;
	camDesc.fFar = 300.f;
	camDesc.fFovy = 60.f;
	camDesc.vAt = { 0.f, 0.f, 1.f, 1.f };
	camDesc.vEye = { 0.f, 2.f, -2.f, 1.f };

	m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Camera", L"Prototype_Free_Camera", &camDesc);

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Player()
{
	m_pGameInstance->Add_Prototype(LEVEL_TOOL, L"Prototype_Model_Player", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Corvus/", "Corvus_No1.dat", "../../Resources/KeyFrame/Player/"));

	m_pGameInstance->Add_Prototype(LEVEL_TOOL, L"Prototype_Model_Player_Dagger", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Corvus/", "Corvus_Dagger.dat"));

	m_pGameInstance->Add_Prototype(LEVEL_TOOL, L"Prototype_Model_Player_Saber", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Corvus/", "Corvus_Saber.dat"));

	m_pGameInstance->Add_Prototype(L"Prototype_Player", CPlayer::Create(m_pDevice, m_pContext));

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Odur()
{
	m_pGameInstance->Add_Prototype(LEVEL_TOOL, L"Prototype_Model_Odur", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Odur/", "Odur.dat", "../../Resources/KeyFrame/Odur/"));

	m_pGameInstance->Add_Prototype(LEVEL_TOOL, L"Prototype_Model_Odur_Cane", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Odur/", "Odur_Cane.dat"));

	m_pGameInstance->Add_Prototype(LEVEL_TOOL, L"Prototype_Model_Odur_Sword", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Odur/", "Odur_Sword.dat"));

	m_pGameInstance->Add_Prototype(LEVEL_TOOL, L"Prototype_Model_Odur_Card", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Odur/", "Odur_Card.dat"));

	m_pGameInstance->Add_Prototype(L"Prototype_Odur_Card", COdur_Card::Create(m_pDevice, m_pContext));

	m_pGameInstance->Add_Prototype(L"Prototype_Odur", COdur::Create(m_pDevice, m_pContext));

	return S_OK;
}

HRESULT CLevel_Tool::Ready_UIResource()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_LockOn", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Target.png"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool::Ready_UI()
{
	return S_OK;
}

HRESULT CLevel_Tool::Ready_Navigation()
{
	m_pGameInstance->Add_Prototype(LEVEL_TOOL, L"Prototype_Navigation", CNavigation::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/NaviData/TestNavi1.dat")));

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
