#include "UI_BossBar.h"

#include "ProgressBar.h"

#include "EnemyStats.h"


CUI_BossBar::CUI_BossBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{

}

CUI_BossBar::CUI_BossBar(const CUI_BossBar& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_BossBar::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CUI_BossBar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_vBossBarScales[LEFT] = { 9.f, 9.f, 1.f };
	m_vBossBarScales[BG] = { 725.f, 9.f, 1.f };
	m_vBossBarScales[RIGHT] = { 9.f, 9.f, 1.f, };

	_float4 vCenter = { 650.f, 50.f, 1.f, 1.f };

	m_vRenderStartPos = { vCenter.x - (m_vBossBarScales[LEFT].x + m_vBossBarScales[BG].x + m_vBossBarScales[RIGHT].x) * 0.5f
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

HRESULT CUI_BossBar::Render()
{
	if (FAILED(Render_MainBar()))
		return E_FAIL;

	if (FAILED(m_pMpProgressBar->Render()))
		return E_FAIL;

	if (FAILED(m_pHpProgressBar->Render()))
		return E_FAIL;

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	Draw_BossNameText();

	return m_pVIBuffer->Bind_Buffers();
}

HRESULT CUI_BossBar::OnEnter_Layer(void* pArg)
{
	m_bReturnToPool = false;

	CEnemyStats* pStats = (CEnemyStats*)pArg;

	m_iMaxHp = pStats->Get_MaxHp();

	pStats->Set_BossBar(this);

	m_wstrBossName = pStats->Get_Name();

	return S_OK;
}

void CUI_BossBar::Update_EnemyHp(_int iHp)
{
	m_pHpProgressBar->Set_Ratio((_float)iHp / m_iMaxHp);
}

void CUI_BossBar::Update_EnemyMp(_int iMp)
{
	m_pHpProgressBar->Set_Ratio((_float)iMp / m_iMaxHp);
}

void CUI_BossBar::Draw_BossNameText()
{
	_float2 vBossTextPos = { m_vRenderStartPos.x, m_vRenderStartPos.y + 5.f };

	m_pGameInstance->Render_Font(L"Main_Font", m_wstrBossName, vBossTextPos);
}


HRESULT CUI_BossBar::Render_MainBar()
{
	_float4 vPivotPos = m_vRenderStartPos;

	for (_uint i = 0; i < BOSSBAR_END; ++i)
	{
		_float4 vRenderPos = { vPivotPos.x + m_vBossBarScales[i].x * 0.5f, vPivotPos.y, 1.f, 1.f };
		m_pTransform->Set_Position(Convert_ScreenToRenderPos(XMLoadFloat4(&vRenderPos)));
		m_pTransform->Set_Scale(m_vBossBarScales[i]);

		if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pBossBarTex[i]->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pVIBuffer->Render()))
			return E_FAIL;

		vPivotPos.x += m_vBossBarScales[i].x;
	}

	return S_OK;
}

HRESULT CUI_BossBar::Ready_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_EnemyBar_Right"), TEXT("Tex_BossBar_Right"), (CComponent**)&m_pBossBarTex[RIGHT])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_EnemyBar_Left"), TEXT("Tex_BossBar_Left"), (CComponent**)&m_pBossBarTex[LEFT])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_EnemyBar_BG"), TEXT("Tex_BossBar_BG"), (CComponent**)&m_pBossBarTex[BG])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
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
	for (_int i = 0; i < BOSSBAR_END; ++i)
		Safe_Release(m_pBossBarTex[i]);

	Safe_Release(m_pHpProgressBar);
	Safe_Release(m_pMpProgressBar);

	__super::Free();
}
