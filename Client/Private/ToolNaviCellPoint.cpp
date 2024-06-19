#include "ToolNaviCellPoint.h"

CToolNaviCellPoint::CToolNaviCellPoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CToolNaviCellPoint::CToolNaviCellPoint(const CToolNaviCellPoint& rhs)
	: CGameObject(rhs)
{
}

HRESULT CToolNaviCellPoint::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CToolNaviCellPoint::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	_float4* vPointPos = (_float4*)pArg;

	m_pTransform->Set_Position(XMLoadFloat4(vPointPos));
	m_pTransform->Set_Scale(_float3(0.5f, 0.5f, 0.5f));

	return S_OK;
}

void CToolNaviCellPoint::Tick(_float fTimeDelta)
{

}

void CToolNaviCellPoint::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CToolNaviCellPoint::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL; 

	if (FAILED(m_pShader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}


HRESULT CToolNaviCellPoint::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform, nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_Cell"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_CellPoint"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}


CToolNaviCellPoint* CToolNaviCellPoint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolNaviCellPoint* pInstance = new CToolNaviCellPoint(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CToolNaviCellPoint"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToolNaviCellPoint::Clone(void* pArg)
{
	CToolNaviCellPoint* pInstance = new CToolNaviCellPoint(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CToolNaviCellPoint"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolNaviCellPoint::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
}
