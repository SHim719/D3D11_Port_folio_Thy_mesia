#include "Main_Camera.h"

#include "GameInstance.h"
#include "Bone.h"
#include "Player.h"

#include "UI_Manager.h"
#include "UI_LockOn.h"

#include "Enemy.h"

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

	_matrix OffsetMatrix = XMMatrixRotationX(-XM_PIDIV2);
	OffsetMatrix *= XMMatrixRotationAxis(OffsetMatrix.r[1], XM_PIDIV2);

	XMStoreFloat4x4(&m_CutsceneOffsetMatrix, OffsetMatrix);

	return S_OK;
}


void CMain_Camera::PriorityTick(_float fTimeDelta)
{
	
}


void CMain_Camera::Tick(_float fTimeDelta)
{
	if (g_hWnd != GetFocus())
		return;

	if (KEY_DOWN(eKeyCode::NUMPAD0))
		m_eState = DEFAULT;
	if (KEY_DOWN(eKeyCode::NUMPAD1))
		m_eState = UI;

	if (nullptr != m_pTarget)
	{
		if (m_pTarget->Is_Death())
		{
			SetState_LockOn_To_Default();
			return;
		}
			
	}

	if (KEY_DOWN(eKeyCode::Q))
	{
		if (LOCKON != m_eState)
			SetState_LockOn();
		else
			SetState_LockOn_To_Default();
	}
}

void CMain_Camera::LateTick(_float fTimeDelta)
{
	Shaking(fTimeDelta);

	switch (m_eState)
	{
	case CMain_Camera::DEFAULT:
		Default_State(fTimeDelta);
		break;
	case CMain_Camera::LOCKON:
		LockOn_State(fTimeDelta);
		break;
	case CMain_Camera::CUTSCENE:
		Cutscene_State(fTimeDelta);
		break;
	case CMain_Camera::UI:
		break;
	}
	
}

void CMain_Camera::Set_Player(CGameObject* pPlayer)
{
	m_pPlayer = (CPlayer*)pPlayer;
	m_pPlayerTransform = pPlayer->Get_Transform();
	Safe_AddRef(m_pPlayerTransform);

	_vector vPlayerPos = m_pPlayerTransform->Get_Position();

	_vector vCameraPos = vPlayerPos + m_pPlayerTransform->Get_Look() * m_vOffset.z + YAXIS * m_vOffset.y;

	m_pTransform->Set_Position(vCameraPos);

	XMStoreFloat4(&m_vPrevTargetPos, m_pPlayerTransform->Get_Position());
}

void CMain_Camera::SetState_LockOn()
{
	CGameObject* pTarget = m_pGameInstance->Find_Target(m_pPlayerTransform->Get_Position());
	if (nullptr == pTarget)
		return;

	m_pTarget = static_cast<CEnemy*>(pTarget);

	m_pTargetTransform = pTarget->Get_Transform();
	
	CModel* pTargetModel = static_cast<CModel*>(pTarget->Find_Component(L"Model"));

	m_pTargetBone = Find_TargetBone(pTargetModel);
	if (nullptr == m_pTargetBone)
		return;
	Safe_AddRef(m_pTargetTransform);
	Safe_AddRef(m_pTargetBone);

	ATTACHDESC Desc = {};
	Desc.pParentTransform = m_pTargetTransform;
	Desc.pAttachBone = m_pTargetBone;

	m_pPlayer->Toggle_LockOn(m_pTargetTransform);
	m_fFollowingSpeed = 4.5f;

	UIMGR->Active_UI("UI_LockOn", &Desc);

	m_eState = LOCKON;
}

void CMain_Camera::SetState_LockOn_To_Default()
{
	if (LOCKON != m_eState)
		return;

	Safe_Release(m_pTargetTransform);
	Safe_Release(m_pTargetBone);

	UIMGR->InActive_UI("UI_LockOn");

	m_pPlayer->Toggle_LockOn();

	m_fFollowingSpeed = 2.5f;

	m_eState = DEFAULT;
}

void CMain_Camera::SetState_Cutscene(const ATTACHDESC& Desc)
{
	SetState_LockOn_To_Default();

	m_pCutsceneTargetTransform = Desc.pParentTransform;
	m_pCutsceneBone = Desc.pAttachBone;

	Safe_AddRef(m_pCutsceneTargetTransform);
	Safe_AddRef(m_pCutsceneBone);

	m_eState = CUTSCENE;
}

void CMain_Camera::Reset_CutsceneState()
{
	XMStoreFloat4(&m_vPrevTargetPos, m_pPlayerTransform->Get_Position());

	Safe_Release(m_pCutsceneTargetTransform);
	Safe_Release(m_pCutsceneBone);

	m_eState = DEFAULT;
}

void CMain_Camera::Add_ShakingDesc(const SHAKINGDESC& ShakingDesc)
{
	XMStoreFloat4(&m_vOriginPos, m_pTransform->Get_Position());

	m_tShakingDesc = ShakingDesc;
	m_bShaking = true;
}

void CMain_Camera::Add_DeltaFovYDesc(const DELTAFOVYDESC& DeltaFovYDesc)
{
	m_tDeltaFovYDesc = DeltaFovYDesc;
	m_bDeltaFovY = true;
	m_bIncreaseFovY = true;
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
	Follow_Target(fTimeDelta);
}

void CMain_Camera::LockOn_State(_float fTimeDelta)
{
	_vector vPlayerPos = m_pPlayerTransform->Get_Position();
	_vector vTargetPos = m_pTargetTransform->Get_Position();
	_vector vCameraLook = XMVector3Normalize(vTargetPos - vPlayerPos);

	_matrix vTargetLookTo = JoMath::LookTo(vCameraLook);

	_matrix ExtraRotationMatrix = XMMatrixRotationAxis(vTargetLookTo.r[0], XMConvertToRadians(25.f));

	vTargetLookTo.r[0] = XMVector3TransformNormal(vTargetLookTo.r[0], ExtraRotationMatrix);
	vTargetLookTo.r[1] = XMVector3TransformNormal(vTargetLookTo.r[1], ExtraRotationMatrix);
	vTargetLookTo.r[2] = XMVector3TransformNormal(vTargetLookTo.r[2], ExtraRotationMatrix);

	_vector vTargetQuat = XMQuaternionRotationMatrix(vTargetLookTo);
	_vector vCurrentQuat = XMQuaternionRotationMatrix(m_pTransform->Get_WorldMatrix());

	_vector vLerpedQuat = XMQuaternionSlerp(vCurrentQuat, vTargetQuat, 5.f * fTimeDelta);

	m_pTransform->Rotation_Quaternion(vLerpedQuat);

	Follow_Target(fTimeDelta);

	Set_CursorToCenter(g_hWnd, g_iWinSizeX, g_iWinSizeY);
}

void CMain_Camera::Cutscene_State(_float fTimeDelta)
{
	m_pTransform->Attach_To_Bone(m_pCutsceneBone, m_pCutsceneTargetTransform, XMLoadFloat4x4(&m_CutsceneOffsetMatrix));


}

void CMain_Camera::Shaking(_float fTimeDelta)
{
	if (false == m_bShaking)
		return;

	m_tShakingDesc.fShakeTime -= fTimeDelta;

	if (m_tShakingDesc.fShakeTime > 0.f)
	{
		_vector vCamPos = XMLoadFloat4(&m_vOriginPos);
		_vector vShakeOffset = 
			XMVectorSet(JoRandom::Random_Float(0.f, 1.f), JoRandom::Random_Float(0.f, 1.f), JoRandom::Random_Float(0.f, 1.f), 0.f)
			* m_tShakingDesc.fShakingForce;

		XMStoreFloat4(&m_vShakingOffset, vShakeOffset);
	}
	else
	{
		m_tShakingDesc.fShakeTime = 0.f;
		XMStoreFloat4(&m_vShakingOffset, XMVectorZero());
		m_bShaking = false;
	}
}

void CMain_Camera::Delta_FovY(_float fTimeDelta)
{
	if (false == m_bDeltaFovY)
		return;

	if (m_bIncreaseFovY)
	{
		m_CameraDesc.fFovy += m_tDeltaFovYDesc.fToTargetSpeed * fTimeDelta;
		if (m_CameraDesc.fFovy >= m_tDeltaFovYDesc.fTargetFovY)
		{
			m_CameraDesc.fFovy = m_tDeltaFovYDesc.fTargetFovY;
			m_bIncreaseFovY = false;
		}
		else
		{

		}
	}

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
	POINT ptMouse = Get_ScreenCursorPos(g_hWnd);
		
	POINT MouseGap = { ptMouse.x - ptCenter.x, ptMouse.y - ptCenter.y };

	_float fDeltaTheta = 0.f;
	if (abs(MouseGap.x) > m_iSensitivity)
	{
		fDeltaTheta = MouseGap.x * fTimeDelta * 0.2f;
		m_pTransform->Add_YAxisInput(fDeltaTheta);
	}
	if (abs(MouseGap.y) > m_iSensitivity)
	{
		fDeltaTheta = MouseGap.y * fTimeDelta * 0.2f;
		m_pTransform->Add_RollInput(fDeltaTheta);
	}

	ClientToScreen(g_hWnd, &ptCenter);
	SetCursorPos(ptCenter.x, ptCenter.y);
}

void CMain_Camera::Follow_Target(_float fTimeDelta)
{
	_vector vLerpedTargetPos = Get_LerpedTargetPos(m_pPlayerTransform->Get_Position(), fTimeDelta);

	XMStoreFloat4(&m_vPrevTargetPos, vLerpedTargetPos);

	_vector vCameraOffset = YAXIS * m_vOffset.y + m_pTransform->Get_Look() * m_vOffset.z;

	_vector vCameraPos = vLerpedTargetPos + vCameraOffset + XMLoadFloat4(&m_vShakingOffset);

	m_pTransform->Set_Position(XMVectorSetW(vCameraPos, 1.f));
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

	Safe_Release(m_pCutsceneTargetTransform);
	Safe_Release(m_pCutsceneBone);
}
