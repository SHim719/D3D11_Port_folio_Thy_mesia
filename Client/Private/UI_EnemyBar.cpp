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

	m_pTransform->Set_Scale({ 150.f, 9.f, 1.f });
	
	CProgressBar::PROGRESSBARDESC BarDesc;
	BarDesc.vScale = { 145.f, 5.f, 1.f };
	BarDesc.wstrTextureTag = L"Prototype_Texture_EnemyBar_MainHpBar";

	m_pHpProgressBar = static_cast<CProgressBar*>(m_pGameInstance->Clone_GameObject(L"Prototype_ProgressBar", &BarDesc));

	BarDesc.wstrTextureTag = L"Prototype_Texture_PlayerMpBar_MainBar";

	m_pMpProgressBar = static_cast<CProgressBar*>(m_pGameInstance->Clone_GameObject(L"Prototype_ProgressBar", &BarDesc));

	UIENEMYBARDESC* pEnemyBarDesc = (UIENEMYBARDESC*)pArg;

	m_pOwnerTransform = pEnemyBarDesc->pOwnerTransform;
	Safe_AddRef(m_pOwnerTransform);

	m_vOffset = pEnemyBarDesc->vOffset;

	m_iMaxHp = pEnemyBarDesc->pStats->Get_MaxHp();

	pEnemyBarDesc->pStats->Set_EnemyBar(this);

	return S_OK;
}

void CUI_EnemyBar::Tick(_float fTimeDelta)
{
	if (m_bStunned)
	{
		m_fAlpha += m_fDeltaAlphaSpeed * fTimeDelta;
		if (0.f >= m_fAlpha || m_fAlpha >= 1.f)
		{
			m_fDeltaAlphaSpeed = -m_fDeltaAlphaSpeed;
			m_fAlpha = clamp(m_fAlpha, 0.f, 1.f);
		}
	}
}

void CUI_EnemyBar::LateTick(_float fTimeDelta)
{
	_vector vEnemyPos = m_pOwnerTransform->Get_Position() + XMLoadFloat4(&m_vOffset);
	_vector vScreenPos = Convert_WorldToScreen(vEnemyPos);

	m_pHpProgressBar->Update_Center(vScreenPos);
	m_pMpProgressBar->Update_Center(vScreenPos);

	XMStoreFloat4(&m_vRenderStartPos, vScreenPos);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
}

HRESULT CUI_EnemyBar::Render()
{
	if (FAILED(Render_MainBar()))
		return E_FAIL;

	if (FAILED(m_pMpProgressBar->Render()))
		return E_FAIL;

	if (FAILED(m_pHpProgressBar->Render()))
		return E_FAIL;

	if (m_bStunned)
	{
		if (FAILED(Render_StunnedShine()))
			return E_FAIL;
	}
		
	return S_OK;
}

void CUI_EnemyBar::Update_EnemyHp(_int iHp)
{
 	m_pHpProgressBar->Set_Ratio((_float)iHp / m_iMaxHp);
}

void CUI_EnemyBar::Update_EnemyMp(_int iMp)
{
	m_pMpProgressBar->Set_Ratio((_float)iMp / m_iMaxHp);
	if (0 == iMp)
	{
		m_bStunned = true;
		m_fAlpha = 1.f;
	}
}

HRESULT CUI_EnemyBar::Render_MainBar()
{
	m_pTransform->Set_Position(Convert_ScreenToRenderPos(XMLoadFloat4(&m_vRenderStartPos)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pUITexture->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_EnemyBar::Render_StunnedShine()
{
	m_pTransform->Set_Position(Convert_ScreenToRenderPos(XMLoadFloat4(&m_vRenderStartPos)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pStunnedShineTex->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_EnemyBar::Ready_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_EnemyBar_BG"), TEXT("Tex_EnemyBar_BG"), (CComponent**)&m_pUITexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_EnemyBar_StunnedSign"), TEXT("Tex_EnemyBar_StunnedSign"), (CComponent**)&m_pStunnedShineTex)))
		return E_FAIL;

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

	Safe_Release(m_pOwnerTransform);

	Safe_Release(m_pHpProgressBar);
	Safe_Release(m_pMpProgressBar);

	Safe_Release(m_pStunnedShineTex);
}
