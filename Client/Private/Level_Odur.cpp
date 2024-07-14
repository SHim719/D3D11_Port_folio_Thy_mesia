#include "Level_Odur.h"
#include "GameInstance.h"

#include "Main_Camera.h"
#include "Player.h"

#include "UI_Manager.h"



CLevel_Odur::CLevel_Odur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CClientLevel(pDevice, pContext)
{
}

HRESULT CLevel_Odur::Initialize()
{
	m_iLevelID = LEVEL_ODUR;

	SET_CURLEVEL(m_iLevelID);

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Player"));
	pPlayer->Set_Active(true);
	pPlayer->Get_Transform()->Set_Position(XMVectorSet(38.193f, 0.887f, -5.502f, 1.f));
	pPlayer->Reset_NaviData(LEVEL_ODUR);
	pPlayer->Enroll_AllColliders();

	UIMGR->Active_UI("UI_PlayerBar");
	UIMGR->Active_UI("UI_PlunderSlot");

	CMain_Camera* pMainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_MainCamera());
	pMainCamera->Set_Active(true);
	pMainCamera->Set_Player(pPlayer);

	if (FAILED(Load_Map(L"../../Resources/Maps/StageOdur/")))
		return E_FAIL;


	//LIGHT_DESC			LightDesc{};
	//LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	//LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vDirection = _float4(1.f, -1.f, 0.f, 0.f);
	//
	//if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
	//	return E_FAIL;


		
	return S_OK;
}

void CLevel_Odur::Tick(_float fTimeDelta)
{

}

HRESULT CLevel_Odur::Render()
{
	return S_OK;
}

CLevel_Odur* CLevel_Odur::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Odur* pInstance = new CLevel_Odur(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Odur"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Odur::Free()
{
	__super::Free();
}
