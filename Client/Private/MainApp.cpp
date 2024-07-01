#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "Levels_Header.h"
#include "Background.h"

#include "ImGui_Main.h"

#include "UI_Manager.h"
#include "Cutscene_Manager.h"

CMainApp::CMainApp()
	: m_pGameInstance { CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}


HRESULT CMainApp::Initialize()
{
	GRAPHIC_DESC		GraphicDesc{};
	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;
	GraphicDesc.isWindowed = true;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;
	
	if (FAILED(Ready_Font()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Change_Level(CLevel_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_pImGui_Main = CImGui_Main::Create(m_pDevice, m_pContext);
	if (nullptr == m_pImGui_Main)
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	m_pGameInstance->Tick_Engine(fTimeDelta);

	m_pImGui_Main->Tick(fTimeDelta);
	m_pImGui_Main->LateTick(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pGameInstance->Draw();

	m_pImGui_Main->Render();

	m_pGameInstance->Present();
	return S_OK;
}


HRESULT CMainApp::Ready_Prototype_Component()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"),
		CVIBuffer_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_VIBuffer_CellPoint"),
		CVIBuffer_Sphere::Create(m_pDevice, m_pContext, 3, 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_AABB"),
		CAABB::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_OBB"),
		COBB::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Sphere"),
		CSphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Shader()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Shader()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNonAnim.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnim.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_UI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_UI.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_Cell"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Font()
{
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Main_Font"), TEXT("../../Resources/Fonts/MainFont11.spritefont"))))
		return E_FAIL;

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{	
	__super::Free();

	CUI_Manager::Destroy_Instance();
	CCutscene_Manager::Destroy_Instance();

	Safe_Release(m_pImGui_Main);
	
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);	

	CGameInstance::Release_Engine();
}

