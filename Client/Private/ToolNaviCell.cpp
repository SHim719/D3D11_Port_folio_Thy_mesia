#include "ToolNaviCell.h"
#include "ToolNaviCellPoint.h"

CToolNaviCell::CToolNaviCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CToolNaviCell::CToolNaviCell(const CToolNaviCell& rhs)
	: CGameObject(rhs)
{
}

HRESULT CToolNaviCell::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CToolNaviCell::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_IdentityMatrix, XMMatrixIdentity());

	return S_OK;
}

void CToolNaviCell::Tick(_float fTimeDelta)
{

}

void CToolNaviCell::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CToolNaviCell::Render()
{

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (nullptr == m_pVIBuffer)
	{
		for (_int i = 0; i < 3; ++i)
			if (nullptr != m_pCellPoints[i])
				m_pCellPoints[i]->Render();
	}
	else
	{
		if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_IdentityMatrix, sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pShader->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(0)))
			return E_FAIL;

		m_pVIBuffer->Render();
	}


	return S_OK;
}


_bool CToolNaviCell::Add_CellPoint(_fvector vPointPos)
{
	_float4 f4PointPos;
	XMStoreFloat4(&f4PointPos, vPointPos);

	CToolNaviCellPoint* pPoint = static_cast<CToolNaviCellPoint*>(m_pGameInstance->Clone_GameObject(L"Prototype_ToolNaviCellPoint", &f4PointPos));
	m_pCellPoints[m_iNumCellPoints++] = pPoint;

	if (3 == m_iNumCellPoints)
	{
		_float3 Points[3] = {};

		XMStoreFloat3(&Points[0], m_pCellPoints[0]->Get_PointPos());
		XMStoreFloat3(&Points[1], m_pCellPoints[1]->Get_PointPos());
		XMStoreFloat3(&Points[2], m_pCellPoints[2]->Get_PointPos());

		m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, Points);

		return true;
	}
	else
		return false;
}


HRESULT CToolNaviCell::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform, nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_Cell"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	return S_OK;
}


CToolNaviCell* CToolNaviCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolNaviCell* pInstance = new CToolNaviCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CToolNaviCell"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToolNaviCell::Clone(void* pArg)
{
	CToolNaviCell* pInstance = new CToolNaviCell(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CToolNaviCell"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolNaviCell::Free()
{
	__super::Free();

	for (_int i = 0; i < 3; ++i)
		Safe_Release(m_pCellPoints[i]);

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
}
