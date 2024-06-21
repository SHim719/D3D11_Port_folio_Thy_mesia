#include "FadeScreen.h"

CFadeScreen::CFadeScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CFadeScreen::CFadeScreen(const CFadeScreen& rhs)
	: CUI(rhs)
{
}

HRESULT CFadeScreen::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFadeScreen::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_FadeScreen"), TEXT("Texture"), (CComponent**)&m_pUITexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	FADEDESC* pFadeDesc = (FADEDESC*)pArg;

	m_eFadeType = pFadeDesc->eFadeType;

	m_vFadeColor = pFadeDesc->eFadeColor == BLACK ? _float4(0.f, 0.f, 0.f, 1.f) : _float4(1.f, 1.f, 1.f, 1.f);

	m_fAlpha = m_eFadeType == FADEOUT ? 0.f : 1.f;
	m_fAlphaDeltaSpeed = pFadeDesc->fAlphaDeltaSpeed;
	m_fExtraTime = pFadeDesc->fExtraTime;

	m_Callback_FadeStart = pFadeDesc->Callback_FadeStart;
	m_Callback_FadeEnd = pFadeDesc->Callback_FadeEnd;

	m_pTransform->Set_Scale({ _float(g_iWinSizeX), _float(g_iWinSizeY), 0.f });

	if (m_Callback_FadeStart)
		m_Callback_FadeStart();

	return S_OK;
}

void CFadeScreen::Tick(_float fTimeDelta)
{
	switch (m_eFadeType)
	{
	case FADEOUT:
		m_fAlpha += m_fAlphaDeltaSpeed * fTimeDelta;
		if (m_fAlpha >= 1.f)
		{
			m_fAlpha = 1.f;

			if (m_fExtraTime > 0.f)
			{
				m_bExtraTime = true;
				break;
			}
			else
			{
				if (m_Callback_FadeEnd)
					m_Callback_FadeEnd();
				m_bDestroyed = true;
			}
		}

		break;
	case FADEIN:
		m_fAlpha -= m_fAlphaDeltaSpeed * fTimeDelta;
		if (m_fAlpha <= 0.f)
		{
			m_fAlpha = 0.f;
			if (m_Callback_FadeEnd)
				m_Callback_FadeEnd();
			m_bDestroyed = true;
		}
		break;
	}
}

void CFadeScreen::LateTick(_float fTimeDelta)
{
	if (m_bExtraTime)
	{
		m_fExtraTime -= fTimeDelta;
		if (m_fExtraTime <= 0.f)
		{
			if (m_Callback_FadeEnd)
				m_Callback_FadeEnd();
			m_bDestroyed = true;
		}
		return;
	}
	
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
}

HRESULT CFadeScreen::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	
	if (FAILED(m_pUITexture->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}



CFadeScreen* CFadeScreen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFadeScreen* pInstance = new CFadeScreen(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CFadeScreen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CFadeScreen::Clone(void* pArg)
{
	CFadeScreen* pInstance = new CFadeScreen(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CFadeScreen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFadeScreen::Free()
{
	__super::Free();

}
