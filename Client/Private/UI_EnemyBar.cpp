#include "UI_EnemyBar.h"

#include "ProgressBar.h"

#include "EnemyStats.h"


CUI_EnemyBar::CUI_EnemyBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{

}

CUI_EnemyBar::CUI_EnemyBar(const CUI_EnemyBar& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_EnemyBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_EnemyBar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_vEnemyBarScales[LEFT] = { 9.f, 9.f, 1.f };
	m_vEnemyBarScales[BG] = { 50.f, 9.f, 1.f };
	m_vEnemyBarScales[RIGHT] = { 9.f, 9.f, 1.f, };
	
	CProgressBar::PROGRESSBARDESC HpBarDesc;
	HpBarDesc.vScale = { 50.f, 9.f, 1.f };
	HpBarDesc.wstrTextureTag = L"Prototype_Texture_PlayerHpBar_MainBar";
	
	m_pHpProgressBar = static_cast<CProgressBar*>(m_pGameInstance->Clone_GameObject(L"Prototype_ProgressBar", &HpBarDesc));

	return S_OK;
}


void CUI_EnemyBar::Tick(_float fTimeDelta)
{

}

void CUI_EnemyBar::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);

}

HRESULT CUI_EnemyBar::Render()
{
	if (FAILED(Render_MainBar()))
		return E_FAIL;

	if (FAILED(m_pHpProgressBar->Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_EnemyBar::Update_EnemyHp(_int iHp)
{
	m_pHpProgressBar->Set_Ratio((_float)iHp / m_iMaxHp);
}

void CUI_EnemyBar::Update_EnemyMp(_int iMp)
{
	m_pMpProgressBar->Set_Ratio((_float)iMp / m_iMaxHp);
}

HRESULT CUI_EnemyBar::Render_MainBar()
{
	_float4 vPivotPos = m_vRenderStartPos;
	
	for (_uint i = 0; i < ENEMYBAR_END; ++i)
	{
		_float4 vRenderPos = { vPivotPos.x + m_vEnemyBarScales[i].x * 0.5f, vPivotPos.y, 1.f, 1.f };
		m_pTransform->Set_Position(Convert_ScreenToWorld(XMLoadFloat4(&vRenderPos)));
		m_pTransform->Set_Scale(m_vEnemyBarScales[i]);
	
		if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
	
		if (FAILED(m_pEnemyBarTex[i]->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
			return E_FAIL;
	
		if (FAILED(m_pShader->Begin(0)))
			return E_FAIL;
	
		if (FAILED(m_pVIBuffer->Render()))
			return E_FAIL;
	
		vPivotPos.x += m_vEnemyBarScales[i].x;
	}

	return S_OK;
}



HRESULT CUI_EnemyBar::Ready_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_EnemyBar_Right"), TEXT("Tex_EnemyBar_Right"), (CComponent**)&m_pEnemyBarTex[RIGHT])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_EnemyBar_Left"), TEXT("Tex_EnemyBar_Left"), (CComponent**)&m_pEnemyBarTex[LEFT])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_EnemyBar_BG"), TEXT("Tex_EnemyBar_BG"), (CComponent**)&m_pEnemyBarTex[BG])))
		return E_FAIL;

	
//Prototype_Texture_EnemyBar_StunnedSign"
//Prototype_Texture_EnemyBar_NewPrestBar"

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}



CUI_EnemyBar* CUI_EnemyBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_EnemyBar* pInstance = new CUI_EnemyBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_EnemyBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_EnemyBar::Clone(void* pArg)
{
	CUI_EnemyBar* pInstance = new CUI_EnemyBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_EnemyBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_EnemyBar::Free()
{
	__super::Free();

	Safe_Release(m_pHpProgressBar);
	Safe_Release(m_pMpProgressBar);

	for (_uint i = 0; i < ENEMYBAR_END; ++i)
	{
		Safe_Release(m_pEnemyBarTex[i]);
		m_pEnemyBarTex[i] = nullptr;
	}
}
