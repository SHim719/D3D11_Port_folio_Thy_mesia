#include "Urd_InversionSphere.h"
#include "GameInstance.h"



CUrd_InversionSphere::CUrd_InversionSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUrd_InversionSphere::CUrd_InversionSphere(const CUrd_InversionSphere& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUrd_InversionSphere::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUrd_InversionSphere::Initialize(void* pArg)
{
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_pGameInstance->Active_ColorInversion();
	return S_OK;
}

void CUrd_InversionSphere::Tick(_float fTimeDelta)
{
	if (m_vNowScale.x < m_fTargetScale)
	{
		_vector vNowScale = XMLoadFloat3(&m_vNowScale);
		_float fScale = m_fScalingSpeed * fTimeDelta;
		vNowScale += XMVectorSet(fScale, fScale, fScale, 0.f);
		if (vNowScale.m128_f32[0] >= m_fTargetScale)
			m_pGameInstance->Inactive_ColorInversion(m_fLerpTime);

		if (vNowScale.m128_f32[0] >= 8.f && 20.f == m_fScalingSpeed)
			m_fScalingSpeed = 250.f;

		XMStoreFloat3(&m_vNowScale, vNowScale);
		m_pTransform->Set_Scale(m_vNowScale);
	}
	else
	{
		m_fLerpTime -= fTimeDelta;
		if (m_fLerpTime <= 0.f)
			Set_Destroy(true);
	}

}

void CUrd_InversionSphere::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_STENCIL, this);
}

HRESULT CUrd_InversionSphere::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pModel->Bind_Buffers(0)))
		return E_FAIL;

	if (FAILED(m_pModel->Render(m_pShader, 0, 2)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CUrd_InversionSphere::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_MeshEffect"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_SMFX_Sphere_01"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

CUrd_InversionSphere* CUrd_InversionSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUrd_InversionSphere* pInstance = new CUrd_InversionSphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUrd_InversionSphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUrd_InversionSphere::Clone(void* pArg)
{
	CUrd_InversionSphere* pInstance = new CUrd_InversionSphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUrd_InversionSphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrd_InversionSphere::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
}
