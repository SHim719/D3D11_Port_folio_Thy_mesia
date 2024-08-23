#include "UI_NeedKey.h"

CUI_NeedKey::CUI_NeedKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_NeedKey::CUI_NeedKey(const CUI_NeedKey& rhs)
	: CUI(rhs)
{
}


HRESULT CUI_NeedKey::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransform->Set_Scale({ 371.f, 39.f, 1.f });
	m_pTransform->Set_Position((Convert_ScreenToRenderPos(XMVectorSet(604.f, 202.f, 1.f, 1.f))));

	m_fAlpha = 1.f;

	return S_OK;
}


void CUI_NeedKey::Tick(_float fTimeDelta)
{
	m_fAlpha = JoMath::Lerp(0.f, 1.f, m_fDuration / 2.f);

	m_fDuration -= fTimeDelta;
	if (m_fDuration < 0.f)
		Set_Destroy(true);
}

void CUI_NeedKey::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
}

HRESULT CUI_NeedKey::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pUITexture->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(3)))
		return E_FAIL;

	return m_pVIBuffer->Render();
}

HRESULT CUI_NeedKey::Ready_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_NeedKey"), TEXT("Texture"), (CComponent**)&m_pUITexture)))
		return E_FAIL;

	return S_OK;
}


CUI_NeedKey* CUI_NeedKey::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_NeedKey* pInstance = new CUI_NeedKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_NeedKey"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_NeedKey::Clone(void* pArg)
{
	CUI_NeedKey* pInstance = new CUI_NeedKey(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_NeedKey"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_NeedKey::Free()
{
	__super::Free();

}
