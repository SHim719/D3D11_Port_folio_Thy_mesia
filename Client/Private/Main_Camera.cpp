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

	
	return S_OK;
}

void CMain_Camera::OnActive()
{
	ShowCursor(FALSE);
	
}

void CMain_Camera::OnInActive()
{
	//ShowCursor(TRUE);
}

void CMain_Camera::PriorityTick(_float fTimeDelta)
{
	
}

void CMain_Camera::Tick(_float fTimeDelta)
{
	if (KEY_DOWN(eKeyCode::NUMPAD0))
		m_eState = DEFAULT;
	if (KEY_DOWN(eKeyCode::NUMPAD1))
		m_eState = UI;

}

void CMain_Camera::LateTick(_float fTimeDelta)
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
	__super::Update_View();
}


void CMain_Camera::Init_TargetPos()
{
	CTransform* pPlayerTransform = m_pTarget->Get_Transform();
	_vector vPlayerNeckPos = Get_PlayerNeckPos();

	_vector vCameraOffset = pPlayerTransform->Get_Right() * m_vOffset.x
		+ pPlayerTransform->Get_Up() * m_vOffset.y - pPlayerTransform->Get_Look() * m_vOffset.z;
	_vector vCameraPosition = Get_PlayerNeckPos() + vCameraOffset;

	m_pTransform->Set_Position(vCameraPosition);

	m_pTransform->LookAt(vPlayerNeckPos);

	m_fOriginArmLength = XMVector3Length(XMLoadFloat4(&m_vOffset)).m128_f32[0];

	XMStoreFloat4(&m_vPrevTargetPos, pPlayerTransform->Get_Position());
} 

void CMain_Camera::Set_Target(CGameObject* pTarget)
{
	Safe_Release(m_pTarget);
	m_pTarget = pTarget;
	Safe_AddRef(m_pTarget);
}

void CMain_Camera::Rotate_CameraArm(_fvector vAxis, _float fDeltaTheta)
{
	_vector vAt = XMLoadFloat4(&m_vPrevTargetPos) + XMLoadFloat4(&m_vPlayerNeckOffset);

	_matrix	RotationMatrix = XMMatrixRotationAxis(vAxis, fDeltaTheta);

	_vector vDist = m_pTransform->Get_Position() - vAt;
	vDist = XMVector3TransformCoord(vDist, RotationMatrix);

	vDist = XMVector3Normalize(vDist) * m_fOriginArmLength;

	m_pTransform->Set_Position(XMVectorSetW(vAt + vDist, 1.f));

	m_pTransform->LookAt(vAt);

	XMStoreFloat4(&m_vCameraArmDist, vDist);
}

void CMain_Camera::Default_State(_float fTimeDelta)
{
	if (nullptr == m_pTarget || g_hWnd != GetFocus())
		return;

	Rotate_By_Mouse(fTimeDelta);
	Follow_Player(fTimeDelta);
}

void CMain_Camera::Follow_Player(_float fTimeDelta)
{
	_vector vPrevPlayerPos = XMLoadFloat4(&m_vPrevTargetPos);
	_vector vPlayerPos = m_pTarget->Get_Transform()->Get_Position();

	_vector vDeltaPlayerPos = vPlayerPos - vPrevPlayerPos;

	if (XMVector3Length(vDeltaPlayerPos).m128_f32[0] < FLT_EPSILON)
		return;

	_vector vLerpedPlayerPos = XMVectorLerp(vPrevPlayerPos, vPlayerPos, m_fFollowingSpeed * fTimeDelta);

	if (XMVector3Length(vPlayerPos - vLerpedPlayerPos).m128_f32[0] < FLT_EPSILON)
		return;

	XMStoreFloat4(&m_vPrevTargetPos, vLerpedPlayerPos);
		
	_vector vAt = vLerpedPlayerPos + XMLoadFloat4(&m_vPlayerNeckOffset);

	m_pTransform->Set_Position(XMVectorSetW(vAt + XMLoadFloat4(&m_vCameraArmDist), 1.f));
}


void CMain_Camera::Rotate_By_Mouse(_float fTimeDelta)
{
	POINT ptCenter = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };
	POINT ptMouse = {};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	POINT MouseGap = { ptMouse.x - ptCenter.x, ptMouse.y - ptCenter.y };

	CTransform* pTargetTransform = m_pTarget->Get_Transform();

	_float fDeltaTheta = 0.f;
	if (abs(MouseGap.x) > m_iSensitivity)
	{
		fDeltaTheta = MouseGap.x * fTimeDelta * 0.2f;
		m_pTransform->Add_YAxisInput(fDeltaTheta);

		Rotate_CameraArm(YAXIS, fDeltaTheta);
	}
	if (abs(MouseGap.y) > m_iSensitivity)
	{
		fDeltaTheta = MouseGap.y * fTimeDelta * 0.2f;
		m_pTransform->Add_RollInput(fDeltaTheta);

		Rotate_CameraArm(m_pTransform->Get_Right(), fDeltaTheta);
	}

	ClientToScreen(g_hWnd, &ptCenter);
	SetCursorPos(ptCenter.x, ptCenter.y);
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
