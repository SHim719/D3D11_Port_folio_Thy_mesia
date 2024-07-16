#include "UI_PlayerBar.h"

#include "ProgressBar.h"

#include "PlayerStats.h"


CUI_PlayerBar::CUI_PlayerBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
	for (_int i = 0; i < PLAYERBAR_END; ++i)
		m_pPlayerHpBarTex[i] = nullptr;
}

CUI_PlayerBar::CUI_PlayerBar(const CUI_PlayerBar& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_PlayerBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PlayerBar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Init_HpBar()))
		return E_FAIL;

	return S_OK;
}


void CUI_PlayerBar::Update_PlayerHp(_int iMaxHp, _int iHp)
{
	m_iPlayerMaxHp = iMaxHp;
	m_iPlayerNowHp = iHp;

	m_pHpProgressBar->Set_Ratio((_float)iHp / iMaxHp);
}

HRESULT CUI_PlayerBar::Render()
{	
	if (FAILED(Render_HpBar()))
		return E_FAIL;

	if (FAILED(m_pHpProgressBar->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PlayerBar::Render_HpBar()
{
	_float4 vPivotPos = m_vHpRenderStartPos;

	for (_uint i = 0; i < PLAYERBAR_END; ++i)
	{
		_float4 vRenderPos = { vPivotPos.x + m_vHpBarScales[i].x * 0.5f, vPivotPos.y, 1.f, 1.f };
		m_pTransform->Set_Position(Convert_ScreenToRenderPos(XMLoadFloat4(&vRenderPos)));
		m_pTransform->Set_Scale(m_vHpBarScales[i]);

		if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pPlayerHpBarTex[i]->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pVIBuffer->Render()))
			return E_FAIL;

		vPivotPos.x += m_vHpBarScales[i].x;
	}

	return S_OK;
}

void CUI_PlayerBar::Draw_HpText()
{
	wstring wstrHpText = to_wstring(m_iPlayerNowHp) + L"/" + to_wstring(m_iPlayerMaxHp);

	m_pGameInstance->Render_Font(L"Main_Font", wstrHpText, m_vHpTextRenderPos, 1.f, XMVectorSet(1.f, 1.f, 1.f, 1.f));
}


HRESULT CUI_PlayerBar::Ready_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_PlayerHpBar_Right"), TEXT("Tex_HpBar_Right"), (CComponent**)&m_pPlayerHpBarTex[RIGHT])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_PlayerHpBar_Left"), TEXT("Tex_HpBar_Left"), (CComponent**)&m_pPlayerHpBarTex[LEFT])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_PlayerHpBar_BG"), TEXT("Tex_HpBar_BG"), (CComponent**)&m_pPlayerHpBarTex[BG])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PlayerBar::Init_HpBar()
{
	//m_vHpBarScales[LEFT] = { 26.f, 11.f, 1.f };
	//m_vHpBarScales[BG] = { 200.f, 11.f, 1.f };
	//m_vHpBarScales[RIGHT] = { 26.f, 11.f, 1.f, };

	m_vHpBarScales[LEFT] = { 26.f, 13.f, 1.f };
	m_vHpBarScales[BG] = { 250.f, 13.f, 1.f };
	m_vHpBarScales[RIGHT] = { 26.f, 13.f, 1.f, };

	_float4 vCenter = { 180.f, (_float)g_iWinSizeY - 60.f, 1.f, 1.f };

	m_vHpRenderStartPos = { vCenter.x - (m_vHpBarScales[LEFT].x + m_vHpBarScales[BG].x + m_vHpBarScales[RIGHT].x) * 0.5f
						 , vCenter.y, 1.f, 1.f };

	CProgressBar::PROGRESSBARDESC HpBarDesc;
	HpBarDesc.vPosition = vCenter;
	//HpBarDesc.vScale = { 215.f, 7.f, 1.f };
	HpBarDesc.vScale = { 265.f, 10.f, 1.f };
	HpBarDesc.wstrTextureTag = L"Prototype_Texture_PlayerHpBar_MainBar";

	m_pHpProgressBar = static_cast<CProgressBar*>(m_pGameInstance->Clone_GameObject(L"Prototype_ProgressBar", &HpBarDesc));

	m_vHpTextRenderPos = { m_vHpRenderStartPos.x + (m_vHpBarScales[LEFT].x + m_vHpBarScales[BG].x + m_vHpBarScales[RIGHT].x) + 10.f,
							m_vHpRenderStartPos.y - m_vHpBarScales[LEFT].y + 2.f };

	return S_OK;
}


CUI_PlayerBar* CUI_PlayerBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_PlayerBar* pInstance = new CUI_PlayerBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_PlayerBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_PlayerBar::Clone(void* pArg)
{
	CUI_PlayerBar* pInstance = new CUI_PlayerBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_PlayerBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PlayerBar::Free()
{
	__super::Free();

	Safe_Release(m_pHpProgressBar);

	for (_uint i = 0; i < PLAYERBAR_END; ++i)
	{
		Safe_Release(m_pPlayerHpBarTex[i]);
		m_pPlayerHpBarTex[i] = nullptr;
	}

}
