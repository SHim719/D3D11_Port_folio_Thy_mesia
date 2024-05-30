#include "Main_Camera.h"

#include "GameInstance.h"
#include "Bone.h"

CMain_Camera::CMain_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CMain_Camera::CMain_Camera(const CMain_Camera& rhs, CTransform::TRANSFORMDESC* pTransformArg)
	: CCamera(rhs, pTransformArg)
{
}


HRESULT CMain_Camera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) 
		return E_FAIL;

	m_pTarget = m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelID(), L"Player", 0);
	Safe_AddRef(m_pTarget);

	CTransform* pPlayerTransform = m_pTarget->Get_Transform();

	_vector vCameraOffset = pPlayerTransform->Get_Right() * m_vPlayerNeckOffset.x
		+ pPlayerTransform->Get_Up() * 3.5f - pPlayerTransform->Get_Look() * 1.5f;
	_vector vCameraPosition = pPlayerTransform->Get_Position() + vCameraOffset;

	m_pTransform->Set_Position(vCameraPosition);
	
	m_pTransform->LookAt(pPlayerTransform->Get_Position() + XMLoadFloat4(&m_vPlayerNeckOffset));
	return S_OK;
}

void CMain_Camera::OnActive()
{
	ShowCursor(FALSE);
}

void CMain_Camera::OnInActive()
{
}

void CMain_Camera::PriorityTick(_float fTimeDelta)
{
	
}

void CMain_Camera::Tick(_float fTimeDelta)
{
	switch (m_eState)
	{
	case CMain_Camera::DEFAULT:
		Default_State(fTimeDelta);
		break;
	case CMain_Camera::LOCKON:
		break;
	case CMain_Camera::CUTSCENE:
		break;
	case CMain_Camera::UI:
		break;
	}

}

void CMain_Camera::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}


void CMain_Camera::Set_Target(CGameObject* pTarget)
{
	Safe_Release(m_pTarget);
	m_pTarget = pTarget;
	Safe_AddRef(m_pTarget);
}

void CMain_Camera::Default_State(_float fTimeDelta)
{
	if (nullptr == m_pTarget || g_hWnd != GetFocus())
		return;

	
	Rotation_ByMouse(fTimeDelta);
}

void CMain_Camera::Rotation_ByMouse(_float fTimeDelta)
{
	POINT MouseGap = m_CurrentMousePos;
	GetCursorPos(&m_CurrentMousePos);
	ScreenToClient(g_hWnd, &m_CurrentMousePos);

	MouseGap.x = m_CurrentMousePos.x - MouseGap.x;
	MouseGap.y = m_CurrentMousePos.y - MouseGap.y;

	CTransform* pTargetTransform = m_pTarget->Get_Transform();
	_vector vAt = pTargetTransform->Get_Position() + XMLoadFloat4(&m_vPlayerNeckOffset);

	_bool bRotate = false;
	if (MouseGap.x != 0.f)
	{
		_float fDeltaTheta = MouseGap.x * fTimeDelta * 0.2f;
		m_pTransform->Add_YAxisInput(fDeltaTheta);

		_vector		vYAxis = XMVectorSet(0.f, 1.0f, 0.f, 0.f);

		_matrix	RotationMatrix = XMMatrixRotationAxis(vYAxis, fDeltaTheta);

		_vector vDist = m_pTransform->Get_Position() - vAt;
		vDist = XMVector3TransformCoord(vDist, RotationMatrix);

		m_pTransform->Set_Position(XMVectorSetW(vAt + vDist, 1.f));

		bRotate = true;
	}
	if (MouseGap.y != 0.f)
	{
		_float fDeltaTheta = MouseGap.y * fTimeDelta * 0.2f;
		m_pTransform->Add_RollInput(fDeltaTheta);

		_vector		vRoll = m_pTransform->Get_Right();

		_matrix	RotationMatrix = XMMatrixRotationAxis(vRoll, fDeltaTheta);

		_vector vDist = m_pTransform->Get_Position() - vAt;
		vDist = XMVector3TransformCoord(vDist, RotationMatrix);

		m_pTransform->Set_Position(XMVectorSetW(vAt + vDist, 1.f));

		bRotate = true;
	}

	if (bRotate)
		m_pTransform->LookAt(vAt);

}


CMain_Camera* CMain_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMain_Camera* pInstance = new CMain_Camera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMain_Camera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMain_Camera::Clone(void* pArg)
{
	CTransform::TRANSFORMDESC* pTransformDesc = (CTransform::TRANSFORMDESC*)pArg;

	CMain_Camera* pInstance = new CMain_Camera(*this, pTransformDesc);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		assert(false);
		MSG_BOX(TEXT("Failed To Cloned : CMain_Camera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMain_Camera::Free()
{
	__super::Free();

	Safe_Release(m_pTarget);
	Safe_Release(m_pCameraBone);
}
