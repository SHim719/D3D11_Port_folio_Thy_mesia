#include "Level_Urd.h"
#include "GameInstance.h"

#include "Main_Camera.h"
#include "Player.h"

#include "UI_Manager.h"



CLevel_Urd::CLevel_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CClientLevel(pDevice, pContext)
{
}

HRESULT CLevel_Urd::Initialize()
{
	m_iLevelID = LEVEL_URD;

	SET_CURLEVEL(m_iLevelID);

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Player"));
	pPlayer->Set_Active(true);
	//pPlayer->Get_Transform()->Set_Position(XMVectorSet(0.f, -14.705f, -82.894f, 1.f));
	pPlayer->Get_Transform()->Set_Position(XMVectorSet(0.f, 0.f, -35.f, 1.f));
	pPlayer->Get_Transform()->LookTo(XMVectorSet(0.f, 0.f, 1.f, 0.f));
	pPlayer->Reset_NaviData(LEVEL_URD);
	pPlayer->Enroll_AllColliders();

	

	UIMGR->Active_UI("UI_PlayerDefault");

	CMain_Camera* pMainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_MainCamera());
	pMainCamera->Set_Active(true);
	pMainCamera->Set_Player(pPlayer);

	if (FAILED(Load_Map(L"../../Resources/Maps/StageUrd/")))
		return E_FAIL;

	LIGHT_DESC			LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vDiffuse = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 0.f, 0.f);
	LightDesc.fLightStrength = 1.f;

	m_pGameInstance->Add_Clone(LEVEL_URD, L"Light", L"Prototype_LightObject", &LightDesc);


	return S_OK;
}

void CLevel_Urd::Tick(_float fTimeDelta)
{

}

HRESULT CLevel_Urd::Render()
{
	return S_OK;
}

CLevel_Urd* CLevel_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Urd* pInstance = new CLevel_Urd(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Urd"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Urd::Free()
{
	__super::Free();
}
