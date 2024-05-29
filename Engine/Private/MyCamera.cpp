#include "MyCamera.h"
#include "PipeLine.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCamera::CCamera(const CCamera& rhs, CTransform::TRANSFORMDESC* pArg)
	: CGameObject(rhs)
{
	m_pTransform = (CTransform*)rhs.m_pTransform->Clone(pArg);
}

HRESULT CCamera::Initialize_Prototype()
{
	m_pTransform = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransform)
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

	m_pTransform->Set_Position(XMLoadFloat4(&m_CameraDesc.vEye));
	m_pTransform->LookAt(XMLoadFloat4(&m_CameraDesc.vAt));

	return S_OK;
}

void CCamera::Tick(_float fTimeDelta)
{
	
}

void CCamera::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransform->Get_WorldMatrixInverse());
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(To_Radian(m_CameraDesc.fFovy), m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
	Safe_Release(m_pGameInstance);
}
