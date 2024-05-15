#include "Frustum.h"

//CFrustum::CFrustum(LPDIRECT3DDEVICE9 pGraphic_Device)
//	: m_pGraphic_Device { pGraphic_Device }
//{
//	Safe_AddRef(m_pGraphic_Device);
//}

HRESULT CFrustum::Initialize()
{
	m_vProjFrustumPoints[0] = { -1.f, 1.f, 0.f };
	m_vProjFrustumPoints[1] = { 1.f, 1.f, 0.f };
	m_vProjFrustumPoints[2] = { 1.f, -1.f, 0.f };
	m_vProjFrustumPoints[3] = { -1.f, -1.f, 0.f };
	m_vProjFrustumPoints[4] = { -1.f, 1.f, 1.f };
	m_vProjFrustumPoints[5] = { 1.f, 1.f, 1.f };
	m_vProjFrustumPoints[6] = { 1.f, -1.f, 1.f };
	m_vProjFrustumPoints[7] = { -1.f, -1.f, 1.f };

	return S_OK;
}

void CFrustum::Tick()
{
	//_float4x4 ViewMatrix, ProjMatrix;
	//m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	//m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);
	//
	//_float4x4 InverseViewMatrix, InverseProjMatrix;
	//
	//D3DXMatrixInverse(&InverseProjMatrix, nullptr, &ProjMatrix);
	//D3DXMatrixInverse(&InverseViewMatrix, nullptr, &ViewMatrix);
	//
	//for (_uint i = 0; i < 8; ++i)
	//{
	//	m_vWorldFrustumPoints[i] = m_vProjFrustumPoints[i];
	//
	//	D3DXVec3TransformCoord(&m_vWorldFrustumPoints[i], &m_vWorldFrustumPoints[i],
	//		&InverseProjMatrix);
	//	D3DXVec3TransformCoord(&m_vWorldFrustumPoints[i], &m_vWorldFrustumPoints[i],
	//		&InverseViewMatrix);
	//}
	//
	//D3DXPlaneFromPoints(&m_WorldFrustumPlane[0], &m_vWorldFrustumPoints[0], &m_vWorldFrustumPoints[1], &m_vWorldFrustumPoints[2]);
	//D3DXPlaneFromPoints(&m_WorldFrustumPlane[1], &m_vWorldFrustumPoints[1], &m_vWorldFrustumPoints[5], &m_vWorldFrustumPoints[6]);
	//D3DXPlaneFromPoints(&m_WorldFrustumPlane[2], &m_vWorldFrustumPoints[4], &m_vWorldFrustumPoints[0], &m_vWorldFrustumPoints[3]);
	//D3DXPlaneFromPoints(&m_WorldFrustumPlane[3], &m_vWorldFrustumPoints[4], &m_vWorldFrustumPoints[5], &m_vWorldFrustumPoints[1]);
	//D3DXPlaneFromPoints(&m_WorldFrustumPlane[4], &m_vWorldFrustumPoints[3], &m_vWorldFrustumPoints[2], &m_vWorldFrustumPoints[6]);
	//D3DXPlaneFromPoints(&m_WorldFrustumPlane[5], &m_vWorldFrustumPoints[5], &m_vWorldFrustumPoints[4], &m_vWorldFrustumPoints[7]);

}

//_bool CFrustum::In_WorldFrustum(_float3 vWorldPos, _float fRadius)
//{
//	_uint iCount = 0;
//	for (_uint i = 0; i < 6; ++i)
//	{
//		if (fRadius >= D3DXPlaneDotCoord(&m_WorldFrustumPlane[i], &vWorldPos))
//			iCount++;
//	}
//
//	if (iCount == 0)
//		return false;
//
//	return true;
//}


//CFrustum* CFrustum::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
//{
//	CFrustum* pInstance = new CFrustum(pGraphic_Device);
//
//	if (FAILED(pInstance->Initialize()))
//	{
//		MSG_BOX(TEXT("Failed to Created : CObject_Manager"));
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//void CFrustum::Free()
//{
//	__super::Free();
//
//	Safe_Release(m_pGraphic_Device);
//}
