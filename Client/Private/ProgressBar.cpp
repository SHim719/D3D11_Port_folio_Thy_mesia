#include "ProgressBar.h"

CProgressBar::CProgressBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CProgressBar::CProgressBar(const CProgressBar& rhs)
	: CUI(rhs)
{
}

HRESULT CProgressBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProgressBar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	PROGRESSBARDESC* pProgressDesc = (PROGRESSBARDESC*)pArg;

	if (FAILED(Ready_Component(pProgressDesc->wstrTextureTag)))
		return E_FAIL;

	m_vOriginScale = pProgressDesc->vScale;
	m_vCenter = pProgressDesc->vPosition;
	m_fLeftX = m_vCenter.x - m_vOriginScale.x * 0.5f;

	return S_OK;
}


HRESULT CProgressBar::Render()
{
	if (0.f == m_fRatio)
		return S_OK;

	Setting_World();

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

void CProgressBar::Setting_World()
{
	_float3 vScale = m_vOriginScale;
	vScale.x *= m_fRatio;
	m_pTransform->Set_Scale(vScale);

	_float4 vPosition = m_vCenter;
	vPosition.x = JoMath::Lerp(m_fLeftX, m_vCenter.x, m_fRatio);
	m_pTransform->Set_Position(Convert_ScreenToWorld(XMLoadFloat4(&vPosition)));
}

void CProgressBar::Update_Center(_vector vCenter)
{
	XMStoreFloat4(&m_vCenter, vCenter);
	m_fLeftX = m_vCenter.x - m_vOriginScale.x * 0.5f;

}

HRESULT CProgressBar::Ready_Component(const wstring& wstrTexTag)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, wstrTexTag, TEXT("Texture"), (CComponent**)&m_pUITexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

CProgressBar* CProgressBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProgressBar* pInstance = new CProgressBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CProgressBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CProgressBar::Clone(void* pArg)
{
	CProgressBar* pInstance = new CProgressBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CProgressBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProgressBar::Free()
{
	__super::Free();

	
}

