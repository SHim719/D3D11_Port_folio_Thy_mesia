#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "Levels_Header.h"
#include "Background.h"

#include "ImGui_Main.h"

#include "UI_Manager.h"
#include "Cutscene_Manager.h"

#include "MapObject.h"
#include "Instancing_Object.h"
#include "Effect_Manager.h"
#include "BossMusic_Player.h"

//#include "Effect_Mesh.h"
//#include "Effect_Particle.h"

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

	if (FAILED(Ready_Default()))
		return E_FAIL;

	CLevel* pStartLevel = nullptr;

#ifndef InGame
	pStartLevel = CLevel_Tool::Create(m_pDevice, m_pContext);

	m_pImGui_Main = CImGui_Main::Create(m_pDevice, m_pContext);
	if (nullptr == m_pImGui_Main)
		return E_FAIL;
#else
	pStartLevel = CLevel_Loading::Create(LEVEL_STAGE1, m_pDevice, m_pContext);

#endif
	m_pGameInstance->Change_Level(pStartLevel);

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	m_pGameInstance->Tick_Engine(fTimeDelta);
	Get_Inst(CBossMusic_Player)->Update();

#ifndef InGame
	m_pImGui_Main->Tick(fTimeDelta);
	m_pImGui_Main->LateTick(fTimeDelta);
#endif

	++m_iFrameCnt;
	m_fFrameAcc += fTimeDelta;
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.3f, 0.3f, 0.3f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pGameInstance->Draw();

#ifndef InGame
	m_pImGui_Main->Render();
#endif

	m_pGameInstance->Present();

	
	if (m_fFrameAcc > 1.f)
	{
		SetWindowTextA(g_hWnd, to_string(m_iFrameCnt).c_str());
		m_fFrameAcc = 0.f;
		m_iFrameCnt = 0;
	}

	return S_OK;
}


HRESULT CMainApp::Ready_Prototype_Component()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Rect_NorTex"),
		CVIBuffer_Rect_NorTex::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"),
		CVIBuffer_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Particle"),
		CVIBuffer_Particle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_VIBuffer_CellPoint"),
		CVIBuffer_Sphere::Create(m_pDevice, m_pContext, 3, 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext))))
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

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Attackable"),
		CAttackable::Create())))
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

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNonAnim.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModelInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNonAnimInstance.hlsl"), VTXMODELINSTANCE_DECLARATION::Elements, VTXMODELINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnim.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_UI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_UI.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements, false))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_Cell"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_Particle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Particle.hlsl"), VTXPARTICLE_DECLARATION::Elements, VTXPARTICLE_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_MeshEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_MeshEffect.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_EffectTrail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_EffectTrail.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_LockOnCurve"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_LockOnCurve.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Font()
{
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Main_Font"), TEXT("../../Resources/Fonts/MainFont11.spritefont"))))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Add_Font(TEXT("Main_Font15"), TEXT("../../Resources/Fonts/MainFont15.spritefont"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Default()
{
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_MapObject", CMapObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_InstancingObj", CInstancing_Object::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	//if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Effect_Particle", CEffect_Particle::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Effect_Mesh", CEffect_Mesh::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

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
	CEffect_Manager::Destroy_Instance();
	CBossMusic_Player::Destroy_Instance();

	Safe_Release(m_pImGui_Main);
	
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);	

	CGameInstance::Release_Engine();
}

