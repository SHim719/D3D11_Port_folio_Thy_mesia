#include "BoxCollider.h"
#include "GameInstance.h"
#include "VIBuffer_Box.h"
#include "GameObject.h"

CBoxCollider::CBoxCollider(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCollider{ pGraphic_Device }
{
}

CBoxCollider::CBoxCollider(const CBoxCollider& rhs)
	: CCollider{ rhs }
{
}

HRESULT CBoxCollider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoxCollider::Initialize(void* pArg)
{
	BOXCOLLISION_DESC desc;
	if (pArg)
	{
		desc = *(BOXCOLLISION_DESC*)pArg;
	}
	else
	{
		desc.vScale = _float3(1.f, 1.f, 1.f);
		desc.vOffset = _float3(0.f, 0.f, 0.f);
	}

	m_pVIBuffer_Box = dynamic_cast<CVIBuffer_Box*>(CGameInstance::Get_Instance()->Clone_Component(0, L"VIBuffer_Box_Default"));
	if (nullptr == m_pVIBuffer_Box)
		return E_FAIL;

	D3DXMatrixIdentity(&m_CollisionMatrix);

	Set_Scale(desc.vScale);
	memcpy(&m_CollisionMatrix.m[3][0], &desc.vOffset, sizeof(_float3));

	m_vOffset = desc.vOffset;

	m_vMin = { -0.5f, -0.5f, -0.5f };
	m_vMax = { 0.5f, 0.5f, 0.5f };

	m_vMin += m_vOffset;
	m_vMax += m_vOffset;
	return S_OK;
}

void CBoxCollider::Render()
{
	m_pGraphic_Device->SetTexture(0, nullptr);
	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_CollisionMatrix);

	m_pVIBuffer_Box->Render();

	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void CBoxCollider::Update_BoxCollider(const _float4x4& Owner_WorldMatrix)
{
	_float3 vOwnerPos;
	memcpy(&vOwnerPos, &Owner_WorldMatrix.m[3], sizeof(_float3));
	vOwnerPos += m_vOffset;

	memcpy(&m_CollisionMatrix.m[3][0], &vOwnerPos, sizeof(_float3));

	D3DXVec3TransformCoord(&m_vMin, &_float3(-0.5f, -0.5f, -0.5f), &m_CollisionMatrix);
	D3DXVec3TransformCoord(&m_vMax, &_float3(0.5f, 0.5f, 0.5f), &m_CollisionMatrix);
}

void CBoxCollider::Set_Scale(const _float3& vScale)
{
	_float3 vRight = _float3(1.f, 0.f, 0.f) * vScale.x;
	_float3 vUp = _float3(0.f, 1.f, 0.f) * vScale.y;
	_float3 vLook = _float3(0.f, 0.f, 1.f) * vScale.z;

	memcpy(&m_CollisionMatrix.m[0][0], &vRight, sizeof(_float3));
	memcpy(&m_CollisionMatrix.m[1][0], &vUp, sizeof(_float3));
	memcpy(&m_CollisionMatrix.m[2][0], &vLook, sizeof(_float3));
}

_float3 CBoxCollider::Get_Scale()
{
	return _float3(m_CollisionMatrix.m[0][0], m_CollisionMatrix.m[1][1], m_CollisionMatrix.m[2][2]);
}


CBoxCollider* CBoxCollider::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBoxCollider* pInstance = new CBoxCollider(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoxCollider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CBoxCollider::Clone(void* pArg)
{
	CBoxCollider* pInstance = new CBoxCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBoxCollider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoxCollider::Free()
{
	__super::Free();

	Safe_Release(m_pVIBuffer_Box);
}
