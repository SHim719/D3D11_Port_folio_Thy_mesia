#include "ToolNaviCell.h"
#include "ToolNaviCellPoint.h"

CToolNaviCell::CToolNaviCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
	XMStoreFloat4x4(&m_IdentityMatrix, XMMatrixIdentity());
}

CToolNaviCell::CToolNaviCell(const CToolNaviCell& rhs)
	: CGameObject(rhs)
{
	XMStoreFloat4x4(&m_IdentityMatrix, XMMatrixIdentity());
}

HRESULT CToolNaviCell::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CToolNaviCell::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_CellPoints = *(vector<CToolNaviCellPoint*>*)pArg;

	for (auto pPoint : m_CellPoints)
		Safe_AddRef(pPoint);

	_float3 vCellPoints[3];
	XMStoreFloat3(&vCellPoints[0], m_CellPoints[0]->Get_PointPos());
	XMStoreFloat3(&vCellPoints[1], m_CellPoints[1]->Get_PointPos());
	XMStoreFloat3(&vCellPoints[2], m_CellPoints[2]->Get_PointPos());

	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, vCellPoints);
	return S_OK;
}

void CToolNaviCell::Tick(_float fTimeDelta)
{
	for (_int i = 0; i < 3; ++i)
	{
		if (m_CellPoints[i]->Is_Updated())
		{
			Remake_VIBuffer();
			return;
		}
	}
}

void CToolNaviCell::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CToolNaviCell::Render()
{
	m_vColor = m_bPicked ? _float4(1.f, 0.f, 0.f, 1.f) : _float4(0.f, 1.f, 0.f, 1.f);

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_IdentityMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(0)))
		return E_FAIL;

	return m_pVIBuffer->Render();
}

_bool CToolNaviCell::Intersect_Ray(_fvector vRayStartPos, _fvector vRayDir)
{
	_float fDist;
	
	return m_bPicked = DirectX::TriangleTests::Intersects(vRayStartPos, vRayDir,
		m_CellPoints[2]->Get_PointPos(), m_CellPoints[1]->Get_PointPos(), m_CellPoints[0]->Get_PointPos(), fDist);
}

void CToolNaviCell::Uncheck_Picking()
{
	m_bPicked = false;

	for (_int i = 0; i < 3; ++i)
		m_CellPoints[i]->Uncheck_Picked();
}


_float3* CToolNaviCell::Get_CellPoints()
{
	_float3* CellPoints = new _float3[3];

	for (size_t i = 0; i < 3; ++i)
		XMStoreFloat3(&CellPoints[i], m_CellPoints[i]->Get_PointPos());

	return CellPoints;
}

void CToolNaviCell::Set_CellPoints(_float3* vPoints)
{
	for (size_t i = 0; i < 3; ++i)
	{
		m_CellPoints[i]->Set_PointPos(XMVectorSetW(XMLoadFloat3(&vPoints[i]), 1.f));
		m_CellPoints[i]->Set_Update(true);
	}
}

void CToolNaviCell::Remake_VIBuffer()
{
	Safe_Release(m_pVIBuffer);

	_float3 vCellPoints[3];
	XMStoreFloat3(&vCellPoints[0], m_CellPoints[0]->Get_PointPos());
	XMStoreFloat3(&vCellPoints[1], m_CellPoints[1]->Get_PointPos());
	XMStoreFloat3(&vCellPoints[2], m_CellPoints[2]->Get_PointPos());

	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, vCellPoints);
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

	for (auto& pPoint : m_CellPoints)
		Safe_Release(pPoint);

	m_CellPoints.clear();

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
}
