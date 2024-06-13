#include "Main_Camera.h"

#include "GameInstance.h"
#include "Bone.h"
#include "Player.h"

#include "UI_Manager.h"
#include "UI_LockOn.h"

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
	
}

void CMain_Camera::OnInActive()
{
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

	if (KEY_DOWN(eKeyCode::Q))
	{
		if (LOCKON != m_eState)
		{
			SetState_LockOn();
			m_pPlayer->Toggle_LockOn(m_pTargetTransform);
		}
		else
		{
			Safe_Release(m_pTargetTransform);
			Safe_Release(m_pTargetBone);

			UIMGR->InActive_UI("UI_LockOn");

			m_pPlayer->Toggle_LockOn();

			m_eState = DEFAULT;
		}
	}

}

void CMain_Camera::LateTick(_float fTimeDelta)
{
	if (g_hWnd != GetFocus())
		return;

	switch (m_eState)
	{
	case CMain_Camera::DEFAULT:
		Default_State(fTimeDelta);
		break;
	case CMain_Camera::LOCKON:
		LockOn_State(fTimeDelta);
		break;
	case CMain_Camera::CUTSCENE:
		break;
	case CMain_Camera::UI:
		break;
	}
	__super::Update_View();
}

void CMain_Camera::Set_Player(CGameObject* pPlayer)
{
	m_pPlayer = (CPlayer*)pPlayer;
	m_pPlayerTransform = pPlayer->Get_Transform();
	Safe_AddRef(m_pPlayerTransform);

	_vector vPlayerPos = m_pPlayerTransform->Get_Position();

	m_pTransform->Set_Position(Calc_OriginCameraPos(vPlayerPos));

	XMStoreFloat4(&m_vPrevTargetPos, m_pPlayerTransform->Get_Position()); 

	m_pTransform->LookAt(Get_PlayerNeckPos(vPlayerPos));

	m_fOriginArmLength = XMVector3Length(XMLoadFloat4(&m_vOffset)).m128_f32[0];
}

void CMain_Camera::SetState_LockOn()
{
	CGameObject* pTarget = m_pGameInstance->Find_Target(m_pPlayerTransform->Get_Position());
	if (nullptr == pTarget)
		return;

	m_pTargetTransform = pTarget->Get_Transform();
	Safe_AddRef(m_pTargetTransform);

	CModel* pTargetModel = static_cast<CModel*>(pTarget->Find_Component(L"Model"));

	m_pTargetBone = Find_TargetBone(pTargetModel);
	if (nullptr == m_pTargetBone)
		return;
	Safe_AddRef(m_pTargetBone);

	CUI_LockOn::LOCKONDESC Desc = {};
	Desc.pTargetTransform = m_pTargetTransform;
	Desc.pTargetBone = m_pTargetBone;

	UIMGR->Active_UI("UI_LockOn", &Desc);

	
	m_eState = LOCKON;
}

CBone* CMain_Camera::Find_TargetBone(CModel* pModel)
{
	if (nullptr == pModel)
		return nullptr;

	CBone* pTargetBone = pModel->Get_Bone("Spine01");
	if (nullptr == pTargetBone)
		pTargetBone = pModel->Get_Bone("Bip001-Spine1");
	if (nullptr == pTargetBone)
		pTargetBone = pModel->Get_Bone("spine_01");
	if (nullptr == pTargetBone)
		pTargetBone = pModel->Get_Bone("Bone");

	return pTargetBone;
}

void CMain_Camera::Default_State(_float fTimeDelta)
{
	Rotate_By_Mouse(fTimeDelta);
	_vector vLerpedTargetPos = Get_LerpedTargetPos(m_pPlayerTransform->Get_Position(), fTimeDelta);

	XMStoreFloat4(&m_vPrevTargetPos, vLerpedTargetPos);

	_vector vAt = vLerpedTargetPos + XMLoadFloat4(&m_vPlayerNeckOffset);

	m_pTransform->Set_Position(XMVectorSetW(vAt + XMLoadFloat4(&m_vCameraArmDist), 1.f));
}

void CMain_Camera::LockOn_State(_float fTimeDelta)
{
	_matrix TargetMatrix = m_pTargetBone->Get_CombinedTransformation() * m_pTargetTransform->Get_WorldMatrix();

	_vector vLerpedTargetPos = Get_LerpedTargetPos(m_pPlayerTransform->Get_Position(), fTimeDelta);
	XMStoreFloat4(&m_vPrevTargetPos, vLerpedTargetPos);

	_vector vCamPos = Calc_OriginCameraPos(vLerpedTargetPos);
	_vector vAt = vLerpedTargetPos + Get_PlayerNeckPos(vLerpedTargetPos);
	XMStoreFloat4(&m_vCameraArmDist, vCamPos - vAt);

	m_pTransform->Set_Position(vCamPos);
	m_pTransform->LookAt(TargetMatrix.r[3]);
}

_vector CMain_Camera::Calc_OriginCameraPos(_vector vPlayerPos)
{
	_vector vPlayerNeckPos = Get_PlayerNeckPos(vPlayerPos);

	_vector vCameraOffset = m_pPlayerTransform->Get_Right() * m_vOffset.x
		+ m_pPlayerTransform->Get_Up() * m_vOffset.y - m_pPlayerTransform->Get_Look() * m_vOffset.z;
	_vector vCameraPosition = vPlayerNeckPos + vCameraOffset;

	return vCameraPosition;
}

_vector CMain_Camera::Get_LerpedTargetPos(_fvector vTargetPos, _float fTimeDelta)
{
	_vector vPrevTargetPos = XMLoadFloat4(&m_vPrevTargetPos);

	_vector vDeltaTargetPos = vTargetPos - vPrevTargetPos;

	if (XMVector3Length(vDeltaTargetPos).m128_f32[0] < FLT_EPSILON)
		return vPrevTargetPos;

	_vector vLerpedTargetPos = XMVectorLerp(vPrevTargetPos, vTargetPos, m_fFollowingSpeed * fTimeDelta);

	if (XMVector3Length(vTargetPos - vLerpedTargetPos).m128_f32[0] < FLT_EPSILON)
		return vTargetPos;

	return vLerpedTargetPos;
}


void CMain_Camera::Rotate_By_Mouse(_float fTimeDelta)
{
	POINT ptCenter = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };
	POINT ptMouse = {};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	POINT MouseGap = { ptMouse.x - ptCenter.x, ptMouse.y - ptCenter.y };

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

void CMain_Camera::Rotate_CameraArm(_fvector vAxis, _float fDeltaTheta)
{
	_vector vAt = XMLoadFloat4(&m_vPrevTargetPos) + XMLoadFloat4(&m_vPlayerNeckOffset);

	_matrix	RotationMatrix = XMMatrixRotationAxis(vAxis, fDeltaTheta);

	_vector vDist = m_pTransform->Get_Position() - vAt;
	vDist = XMVector3TransformCoord(vDist, RotationMatrix);

	vDist = XMVector3Normalize(vDist) * m_fOriginArmLength;

	XMStoreFloat4(&m_vCameraArmDist, vDist);

	m_pTransform->Set_Position(XMVectorSetW(vAt + vDist, 1.f));

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

	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pTargetTransform);
	Safe_Release(m_pTargetBone);
	
}
