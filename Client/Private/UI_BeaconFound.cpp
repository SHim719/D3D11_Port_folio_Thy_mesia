#include "UI_BeaconFound.h"

CUI_BeaconFound::CUI_BeaconFound(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_BeaconFound::CUI_BeaconFound(const CUI_BeaconFound& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_BeaconFound::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_BeaconFound::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_vBGScale = { 1280.f, 200.f, 0.f };
	m_vMessageScale = { 480.f, 70.f, 0.f };

	return S_OK;
}


void CUI_BeaconFound::Tick(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Appear:
		m_fAlpha += m_fDeltaAlphaSpeed * fTimeDelta;
		if (m_fAlpha >= 1.f)
		{
			m_fAlpha = 1.f;
			m_eState = Idle;
		}
		break;
	case Idle:
		m_fTimeAcc += fTimeDelta;
		if (m_fTimeAcc >= m_fDuration)
		{
			m_fTimeAcc = 0.f;
			m_eState = Disappear;
		}
		break;
	case Disappear:
		m_fAlpha -= m_fDeltaAlphaSpeed * fTimeDelta;
		if (m_fAlpha <= 0.f)
		{
			m_fAlpha = 0.f;
			m_bReturnToPool = true;
		}
		break;
	}

	m_fNowScaling += m_fScalingSpeed * fTimeDelta;
}

void CUI_BeaconFound::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);

}

HRESULT CUI_BeaconFound::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(Render_BG()))
		return E_FAIL;

	if (FAILED(Render_Message()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_BeaconFound::OnEnter_Layer(void* pArg)
{
	if (FAILED(__super::OnEnter_Layer(pArg)))
		return E_FAIL;

	m_fAlpha = 0.f;
	m_fTimeAcc = 0.f;
	m_fNowScaling = 1.f;

	m_eState = Appear;

	return S_OK;
}

HRESULT CUI_BeaconFound::Render_BG()
{
	m_pTransform->Set_Scale(m_vBGScale);

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	
	if (FAILED(m_pBGTexture->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_BeaconFound::Render_Message()
{
	_vector vMessageScale = XMLoadFloat3(&m_vMessageScale) * m_fNowScaling;

	_float3 vNowScaling;
	XMStoreFloat3(&vNowScaling, vMessageScale);

	m_pTransform->Set_Scale(vNowScaling);

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pUITexture->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}


HRESULT CUI_BeaconFound::Ready_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_BeaconFound"), TEXT("Beacon_Texture"), (CComponent**)&m_pUITexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_LandingScreen_BG"), TEXT("BG_Texture"), (CComponent**)&m_pBGTexture)))
		return E_FAIL;

	return S_OK;
}

CUI_BeaconFound* CUI_BeaconFound::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_BeaconFound* pInstance = new CUI_BeaconFound(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_BeaconFound"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_BeaconFound::Clone(void* pArg)
{
	CUI_BeaconFound* pInstance = new CUI_BeaconFound(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_BeaconFound"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_BeaconFound::Free()
{
	__super::Free();

	Safe_Release(m_pBGTexture);
}
