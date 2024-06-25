#include "UI_BossBar.h"

#include "ProgressBar.h"

#include "EnemyStats.h"


CUI_BossBar::CUI_BossBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_EnemyBar(pDevice, pContext)
{

}

CUI_BossBar::CUI_BossBar(const CUI_BossBar& rhs)
	: CUI_EnemyBar(rhs)
{
}

HRESULT CUI_BossBar::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CUI_BossBar::Initialize(void* pArg)
{
	if (FAILED(CUI::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(__super::Ready_Component()))
		return E_FAIL;

	m_vEnemyBarScales[LEFT] = { 9.f, 9.f, 1.f };
	m_vEnemyBarScales[BG] = { 725.f, 9.f, 1.f };
	m_vEnemyBarScales[RIGHT] = { 9.f, 9.f, 1.f, };

	_float4 vCenter = { 650.f, 50.f, 1.f, 1.f };

	m_vRenderStartPos = { vCenter.x - (m_vEnemyBarScales[LEFT].x + m_vEnemyBarScales[BG].x + m_vEnemyBarScales[RIGHT].x) * 0.5f
						 , vCenter.y, 1.f, 1.f };


	CProgressBar::PROGRESSBARDESC HpBarDesc;
	HpBarDesc.vPosition = vCenter;
	HpBarDesc.vScale = { 725.f, 5.f, 1.f };
	HpBarDesc.wstrTextureTag = L"Prototype_Texture_EnemyBar_MainHpBar";

	m_pHpProgressBar = static_cast<CProgressBar*>(m_pGameInstance->Clone_GameObject(L"Prototype_ProgressBar", &HpBarDesc));

	HpBarDesc.wstrTextureTag = L"Prototype_Texture_PlayerMpBar_MainBar";

	m_pMpProgressBar = static_cast<CProgressBar*>(m_pGameInstance->Clone_GameObject(L"Prototype_ProgressBar", &HpBarDesc));


	return S_OK;
}


void CUI_BossBar::Tick(_float fTimeDelta)
{

}

void CUI_BossBar::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);

}

HRESULT CUI_BossBar::Render()
{
	if (FAILED(Render_MainBar()))
		return E_FAIL;

	if (FAILED(m_pHpProgressBar->Render()))
		return E_FAIL;

	if (FAILED(m_pMpProgressBar->Render()))
		return E_FAIL;

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	Draw_BossNameText();

	return S_OK;
}

HRESULT CUI_BossBar::OnEnter_Layer(void* pArg)
{
	CEnemyStats* pStats = (CEnemyStats*)pArg;

	m_iMaxHp = pStats->Get_MaxHp();

	pStats->Add_Observer(this);

	m_wstrBossName = pStats->Get_Name();

	return S_OK;
}


void CUI_BossBar::Draw_BossNameText()
{
	_float2 vBossTextPos = { m_vRenderStartPos.x, m_vRenderStartPos.y + 5.f };

	m_pGameInstance->Render_Font(L"Main_Font", m_wstrBossName, vBossTextPos, 1.f, XMVectorSet(1.f, 1.f, 1.f, 1.f));

}


CUI_BossBar* CUI_BossBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_BossBar* pInstance = new CUI_BossBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_BossBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_BossBar::Clone(void* pArg)
{
	CUI_BossBar* pInstance = new CUI_BossBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_BossBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_BossBar::Free()
{
	__super::Free();
}
