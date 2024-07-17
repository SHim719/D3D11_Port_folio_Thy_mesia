#include "UI_Menu.h"

#include "UI_Manager.h"

#include "FadeScreen.h"


CUI_Menu::CUI_Menu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Menu::CUI_Menu(const CUI_Menu& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Menu::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Menu::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	Init_UIMatrices();

	return S_OK;
}


void CUI_Menu::Tick(_float fTimeDelta)
{
	if (KEY_DOWN(eKeyCode::Up))
	{
		m_eNowMenu = Menu((m_eNowMenu + MENU_END - 1) % MENU_END); 
		m_pTransform->Set_Position(XMLoadFloat4(&m_vFontPos[m_eNowMenu]));
	}
		
	else if (KEY_DOWN(eKeyCode::Down))
	{
		m_eNowMenu = Menu((m_eNowMenu + 1) % MENU_END);
		m_pTransform->Set_Position(XMLoadFloat4(&m_vFontPos[m_eNowMenu]));
	}

	else if (KEY_DOWN(eKeyCode::Enter))
	{
		Select_Menu();
	}
		
}

void CUI_Menu::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Menu::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(Render_BG()))
		return E_FAIL;

	if (FAILED(Render_ThymesiaFont()))
		return E_FAIL;

	if (FAILED(Render_Highlighted()))
		return E_FAIL;

	if (FAILED(Render_Fonts()))
		return E_FAIL;


	return S_OK;
}

void CUI_Menu::Select_Menu()
{
	switch (m_eNowMenu)
	{
	case LEVEL_UP:
		Open_Window("UI_Stats");
		break;
	case PLAGUE_WEAPON:
		Open_Window("UI_PW");
		break;
	case RESUME:
		m_eNowMenu = LEVEL_UP;
		UIMGR->Inactive_Menu();
		break;
	}
}

HRESULT CUI_Menu::Render_BG()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_BGWorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pUITexture->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(3)))
		return E_FAIL;

	return m_pVIBuffer->Render();
}

HRESULT CUI_Menu::Render_Highlighted()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pMenuHighlightedTex->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(3)))
		return E_FAIL;

	return m_pVIBuffer->Render();
}

HRESULT CUI_Menu::Render_ThymesiaFont()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_ThymesiaFontMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pThymesiaFontTex->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(3)))
		return E_FAIL;

	return m_pVIBuffer->Render();
}

HRESULT CUI_Menu::Render_Fonts()
{
	_float fAlpha = 1.f;

	for (_uint i = 0; i < MENU_END; ++i)
	{
		fAlpha = m_eNowMenu == i ? 1.f : 0.5f;

		if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_FontMatrix[i], sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pMenuFontTextures[i]->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(3)))
			return E_FAIL;

		if (FAILED(m_pVIBuffer->Render()))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CUI_Menu::Ready_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_MainMenu_Left"), TEXT("Menu_Texture"), (CComponent**)&m_pUITexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_MainMenu_Highlighted"), TEXT("Highlighted_Texture"), (CComponent**)&m_pMenuHighlightedTex)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Font_Thymesia"), TEXT("Texture_Font0"), (CComponent**)&m_pThymesiaFontTex)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Font_LevelUp"), TEXT("Texture_Font1"), (CComponent**)&m_pMenuFontTextures[LEVEL_UP])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Font_PW"), TEXT("Texture_Font2"), (CComponent**)&m_pMenuFontTextures[PLAGUE_WEAPON])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Font_Resume"), TEXT("Texture_Font3"), (CComponent**)&m_pMenuFontTextures[RESUME])))
		return E_FAIL;

	return S_OK;
}

void CUI_Menu::Init_UIMatrices()
{
	_matrix WorldMatrix = XMMatrixIdentity();
	_float3 vScale = { 589.f, 1467.f, 0.f };

	WorldMatrix.r[0] *= vScale.x;
	WorldMatrix.r[1] *= vScale.y;
	WorldMatrix.r[3] = Convert_ScreenToRenderPos(XMVectorSet(vScale.x * 0.5f, 720.f * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_BGWorldMatrix, XMMatrixTranspose(WorldMatrix));

	WorldMatrix = XMMatrixIdentity();
	vScale = { 129.f, 29.f, 0.f };
	//vScale = { 200.f, 58.f, 0.f };

	WorldMatrix.r[0] *= vScale.x;
	WorldMatrix.r[1] *= vScale.y;
	WorldMatrix.r[3] = Convert_ScreenToRenderPos(XMVectorSet(110.f, 81.f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ThymesiaFontMatrix, XMMatrixTranspose(WorldMatrix));

	WorldMatrix = XMMatrixIdentity();
	vScale = { 66.f, 20.f, 0.f };

	WorldMatrix.r[0] *= vScale.x;
	WorldMatrix.r[1] *= vScale.y;
	WorldMatrix.r[3] = Convert_ScreenToRenderPos(XMVectorSet(107.f, 156.f, 0.f, 1.f));

	XMStoreFloat4(&m_vFontPos[LEVEL_UP], WorldMatrix.r[3]);
	XMStoreFloat4x4(&m_FontMatrix[LEVEL_UP], XMMatrixTranspose(WorldMatrix));

	WorldMatrix = XMMatrixIdentity();
	vScale = { 87.f, 20.f, 0.f };

	WorldMatrix.r[0] *= vScale.x;
	WorldMatrix.r[1] *= vScale.y;
	WorldMatrix.r[3] = Convert_ScreenToRenderPos(XMVectorSet(109.f, 253.f, 0.f, 1.f));

	XMStoreFloat4(&m_vFontPos[PLAGUE_WEAPON], WorldMatrix.r[3]);
	XMStoreFloat4x4(&m_FontMatrix[PLAGUE_WEAPON], XMMatrixTranspose(WorldMatrix));

	WorldMatrix = XMMatrixIdentity();
	vScale = { 88.f, 20.f, 0.f };

	WorldMatrix.r[0] *= vScale.x;
	WorldMatrix.r[1] *= vScale.y;
	WorldMatrix.r[3] = Convert_ScreenToRenderPos(XMVectorSet(109.f, 352.f, 0.f, 1.f));

	XMStoreFloat4(&m_vFontPos[RESUME], WorldMatrix.r[3]);
	XMStoreFloat4x4(&m_FontMatrix[RESUME], XMMatrixTranspose(WorldMatrix));

	m_pTransform->Set_Scale({ 388.f, 43.f, 0.f });
	m_pTransform->Set_Position(XMLoadFloat4(&m_vFontPos[LEVEL_UP]));
}

void CUI_Menu::Open_Window(const string& strUI)
{
	CFadeScreen::FADEDESC FadeDesc{};
	FadeDesc.eFadeColor = CFadeScreen::BLACK;
	FadeDesc.fFadeOutSpeed = 3.f;
	FadeDesc.fFadeInSpeed = 10.f;
	FadeDesc.pCallback_FadeOutEnd = bind(&CUI_Manager::Active_UI, CUI_Manager::Get_Instance(), strUI, nullptr);
	FadeDesc.pCallback_FadeInStart = bind(&CGameObject::Set_ReturnToPool, this, true);

	UIMGR->Active_UI("FadeScreen", &FadeDesc);
}

CUI_Menu* CUI_Menu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Menu* pInstance = new CUI_Menu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Menu"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_Menu::Clone(void* pArg)
{
	CUI_Menu* pInstance = new CUI_Menu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Menu"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Menu::Free()
{
	__super::Free();

	Safe_Release(m_pMenuHighlightedTex);
	Safe_Release(m_pThymesiaFontTex);

	for (_uint i = 0; i < MENU_END; ++i)
		Safe_Release(m_pMenuFontTextures[i]);
}
