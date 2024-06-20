#include "Navigation.h"
#include "Cell.h"

#include "GameInstance.h"

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
	, m_Cells{ rhs.m_Cells }
	, m_iCurrentIndex{ rhs.m_iCurrentIndex }
#ifdef _DEBUG
	, m_pShader{ rhs.m_pShader }
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif
	Safe_AddRef(m_pGameInstance);

	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);
}

HRESULT CNavigation::Initialize_Prototype(const wstring& strNavigationDataFile)
{
	ifstream fin(strNavigationDataFile, ios::binary);

	if (!fin.is_open())
		return E_FAIL;

	_int iReserveSize = 0;

	fin.read((_char*)&iReserveSize, sizeof(_int));

	m_Cells.reserve(iReserveSize);
	
	while (true)
	{
		_float3		vPoints[CCell::POINT_END] = {};

		fin.read((_char*)vPoints, sizeof(_float3) * CCell::POINT_END);
		if (fin.eof())
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_int)m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.emplace_back(pCell);
	}
	SetUp_Neighbors();

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		CNavigation::NAVIGATION_DESC* pDesc = static_cast<CNavigation::NAVIGATION_DESC*>(pArg);

		m_iCurrentIndex = pDesc->iCurrentCellIndex;
	}

	return S_OK;
}

HRESULT CNavigation::Render()
{
#ifdef _DEBUG
	_float4x4 IdentityMatrix;
	XMStoreFloat4x4(&IdentityMatrix, XMMatrixIdentity());

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &IdentityMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (-1 == m_iCurrentIndex)
	{
		m_vColor.x = 0.f;
		m_vColor.y = 1.f;
		
		if (FAILED(m_pShader->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
			return E_FAIL;

		m_pShader->Begin(0);

		for (auto& pCell : m_Cells)
			pCell->Render();
	}
	else
	{
		m_vColor.x = 1.f;
		m_vColor.y = 0.f;
		
		if (FAILED(m_pShader->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
			return E_FAIL;

		m_pShader->Begin(0);

		m_Cells[m_iCurrentIndex]->Render();
	}


#endif

	return S_OK;
}

_bool CNavigation::isMove(_fvector vPosition, _float4* pNormal)
{
	if (-1 == m_iCurrentIndex || m_iCurrentIndex >= m_Cells.size())
		return false;

	/* Cell안에 있는 점 세개가 로컬에 존재, 인자로 받아온 vPosition 월드 위치. 싱크를 맞춰야된다. */
	/* vPosition -> LocalPos로 바꾸는 작업을 수행하자. */
	//_vector		vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	_int		iNeighborIndex = { -1 };

	/* 움직이고난 결과가 현재 존재한던 셀안에존재한다. == 현재 셀 안에서 움직인거다. */
	if (true == m_Cells[m_iCurrentIndex]->isIn(vPosition, &iNeighborIndex, pNormal))
		return true;

	else /* 현재 셀을 벗어났다. */
	{
		/* 벗어난 방향에 이웃이 없으면. */
		if (-1 == iNeighborIndex)
			return false;

		/* 벗어난 방향에 이웃이 있으면. */
		else
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex, pNormal))
					break;
			}

			m_iCurrentIndex = iNeighborIndex;
			return true;
		}
	}
}

_float CNavigation::Decide_YPos(_fvector vPosition)
{
	return m_Cells[m_iCurrentIndex]->Calc_YPos(vPosition);
}


HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);
			}
			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);
			}
			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);
			}
		}
	}

	return S_OK;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strNavigationDataFile)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strNavigationDataFile)))
	{
		MSG_BOX(TEXT("Failed to Created : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);
}
