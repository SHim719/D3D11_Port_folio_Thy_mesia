#include "Level_Stage1.h"
#include "GameInstance.h"

#include "Main_Camera.h"
#include "Player.h"

#include "UI_Manager.h"


CLevel_Stage1::CLevel_Stage1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CClientLevel(pDevice, pContext)
{
}

HRESULT CLevel_Stage1::Initialize()
{
    m_iLevelID = LEVEL_STAGE1;

	SET_CURLEVEL(m_iLevelID);

	CCamera::CAMERADESC camDesc{};
	camDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	camDesc.fNear = 0.1f;
	camDesc.fFar = 100.f;
	camDesc.fFovy = 70.f;
	camDesc.vAt = { 0.f, 0.f, 1.f, 1.f };
	camDesc.vEye = { 0.f, 2.f, -2.f, 1.f };

	CMain_Camera* pMainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Clone_GameObject(L"Prototype_Main_Camera", &camDesc));
	m_pGameInstance->Change_MainCamera(pMainCamera);

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Player", L"Prototype_Player"));
	//pPlayer->Get_Transform()->Set_Position(XMVectorSet(0.f, 0.f, 0.f, 1.f));
	pPlayer->Get_Transform()->Set_Position(XMVectorSet(-32.614f, -2.014f, 116.430f, 1.f));
	pPlayer->Reset_NaviData(LEVEL_STAGE1);

	pMainCamera->Set_Player(pPlayer);

	if (FAILED(UIMGR->Initialize(m_pDevice, m_pContext)))
		return E_FAIL;

 	if (FAILED(Load_Map(L"../../Resources/Maps/Stage1/")))
		return E_FAIL;

	LIGHT_DESC			LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vDiffuse = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 0.f, 0.f);
	LightDesc.fLightStrength = 1.f;
	
	m_pGameInstance->Add_Clone(LEVEL_STAGE1, L"Light", L"Prototype_LightObject", &LightDesc);

	UIMGR->Active_UI("UI_PlayerDefault");

	m_wstrBGMTag = L"Stage1BGM";

	PLAY_SOUND(m_wstrBGMTag, true, 1.f);

	return S_OK;
}

void CLevel_Stage1::Tick(_float fTimeDelta)
{
	if (KEY_DOWN(eKeyCode::L))
		Ready_ChangeLevel(LEVEL_URD);
}

HRESULT CLevel_Stage1::Render()
{
	return S_OK;
}

CLevel_Stage1* CLevel_Stage1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Stage1* pInstance = new CLevel_Stage1(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Stage1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage1::Free()
{
	__super::Free();
}
