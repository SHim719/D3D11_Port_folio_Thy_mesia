#include "Main_Camera.h"

#include "GameInstance.h"
#include "Bone.h"
#include "Player.h"

#include "UI_Manager.h"
#include "UI_LockOn.h"

#include "Enemy.h"

#include "LockOnCurve.h"

#include "Layer.h"

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

	Init_CameraShakingDescs();

	_matrix OffsetMatrix = XMMatrixRotationX(-XM_PIDIV2);
	OffsetMatrix *= XMMatrixRotationAxis(OffsetMatrix.r[1], XM_PIDIV2);

	XMStoreFloat4x4(&m_CutsceneOffsetMatrix, OffsetMatrix);

	Set_CursorToCenter(g_hWnd, g_iWinSizeX, g_iWinSizeY);

	m_pLockOnCurve = static_cast<CLockOnCurve*>(m_pGameInstance->Clone_GameObject(L"Prototype_LockOnCurve"));

	return S_OK;
}

void CMain_Camera::Init_CameraShakingDescs()
{
	CMain_Camera::SHAKINGDESC ShakingDesc;
	ShakingDesc.fShakeTime = 0.1f;
	ShakingDesc.fShakingForce = 0.1f;

	m_ShakingDescs.emplace("Shaking_Hit", ShakingDesc);

	ShakingDesc.fShakeTime = 0.1f;
	ShakingDesc.fShakingForce = 0.4f;
	m_ShakingDescs.emplace("Shaking_Execution", ShakingDesc);

	ShakingDesc.fShakeTime = 0.15f;
	ShakingDesc.fShakingForce = 0.2f;
	m_ShakingDescs.emplace("Shaking_Claw_Long", ShakingDesc);

	ShakingDesc.fShakeTime = 0.3f;
	ShakingDesc.fShakingForce = 1.f;
	m_ShakingDescs.emplace("Shaking_PW_Hammer", ShakingDesc);

	ShakingDesc.fShakeTime = 0.5f;
	ShakingDesc.fShakingForce = 1.5f;
	m_ShakingDescs.emplace("Shaking_Joker_Impact", ShakingDesc);

	ShakingDesc.fShakeTime = 0.1f;
	ShakingDesc.fShakingForce = 0.2f;
	m_ShakingDescs.emplace("Shaking_Parry_Normal", ShakingDesc);

	ShakingDesc.fShakeTime = 0.1f;
	ShakingDesc.fShakingForce = 0.3f;
	m_ShakingDescs.emplace("Shaking_Parry_Big", ShakingDesc);

}

void CMain_Camera::OnActive()
{
	Set_CursorToCenter(g_hWnd, g_iWinSizeX, g_iWinSizeY);
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
	Safe_Release(m_pPlayerTransform);
	m_pTransform->Rotation_Quaternion(XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f));

	m_pPlayer = (CPlayer*)pPlayer;
	m_pPlayerTransform = pPlayer->Get_Transform();
	Safe_AddRef(m_pPlayerTransform);

	_vector vPlayerPos = m_pPlayerTransform->Get_Position();

	_vector vCameraPos = vPlayerPos + m_pPlayerTransform->Get_Look() * m_vOffset.z + YAXIS * m_vOffset.y;

	m_pTransform->Set_Position(vCameraPos);

	XMStoreFloat4(&m_vPrevTargetPos, m_pPlayerTransform->Get_Position());
}

void CMain_Camera::SetState_UI()
{
	m_eState = UI;
}

void CMain_Camera::SetState_Default()
{
	m_eState = DEFAULT;
}

void CMain_Camera::SetState_LockOn()
{
	CGameObject* pTarget = Find_LockOnTarget();
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
	m_fFollowingSpeed = m_fLockonFollowingSpeed;

	UIMGR->Active_UI("UI_LockOn", &Desc);
	m_pLockOnCurve->OnEnter_Layer(nullptr);
	Safe_AddRef(m_pLockOnCurve);
	ADD_EVENT(bind(&CGameInstance::Insert_GameObject, m_pGameInstance, GET_CURLEVEL, L"LockOnCurve", m_pLockOnCurve));

	m_eState = LOCKON;
}

void CMain_Camera::SetState_LockOn_To_Default()
{
	if (LOCKON != m_eState)
		return;

	Safe_Release(m_pTargetTransform);
	Safe_Release(m_pTargetBone);

	UIMGR->Inactive_UI("UI_LockOn");

	m_pPlayer->Toggle_LockOn();

	m_fFollowingSpeed = m_fFollowingSpeed;

	m_pTarget = nullptr;

	m_pLockOnCurve->Set_ReturnToPool(true);

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

void CMain_Camera::Play_CameraShake(const string& strTag)
{
	auto it = m_ShakingDescs.find(strTag);
	if (m_ShakingDescs.end() == it)
		return;

	if (m_tShakingDesc.fShakeTime > 0.f && m_tShakingDesc.fShakingForce > it->second.fShakingForce)
		return;

	m_tShakingDesc = it->second;
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

void CMain_Camera::Update_LockOnCurveDesc()
{
	CLockOnCurve::CURVE_DESCS CurveDescs;

	CALC_TF->Set_WorldMatrix(m_pPlayerTransform->Get_WorldMatrix());
	CALC_TF->Add_Position(XMVectorSet(-0.1f, 1.2f, 0.f, 0.f), true);

	CurveDescs.PlayerWorldMatrix = CALC_TF->Get_WorldFloat4x4_TP();
	
	CALC_TF->Attach_To_Bone(m_pTargetBone, m_pTargetTransform);

	XMStoreFloat4(&CurveDescs.vTargetPos, CALC_TF->Get_Position());

	m_pLockOnCurve->Set_CurveDescs(CurveDescs);
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
	Update_LockOnCurveDesc();

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

CGameObject* CMain_Camera::Find_LockOnTarget()
{
	CLayer* pEnemyLayer = m_pGameInstance->Find_Layer(GET_CURLEVEL, L"Enemy");

	if (nullptr == pEnemyLayer)
		return nullptr;
		
	_float fMinDist = 999999.f;
	CGameObject* pTarget = nullptr;

	_vector vCameraLook = m_pTransform->Get_GroundLook();
	_vector vCameraPos = m_pTransform->Get_Position();
	_vector vPlayerPos = m_pPlayerTransform->Get_Position();

	auto& GameObjects = pEnemyLayer->Get_GameObjects();
	
	for (auto& pGameObject : GameObjects)
	{
		CEnemy* pEnemy = static_cast<CEnemy*>(pGameObject);
		if (false == pEnemy->Is_Active() || true == pEnemy->Is_OutOfLayer() ||
			pEnemy->Is_Death())
			continue;

		_vector vTargetPos = pGameObject->Get_Transform()->Get_Position();

		_float fDist = XMVector3Length(vTargetPos - vPlayerPos).m128_f32[0];

		if (fDist > 10.f)
			continue;

		if (fDist < fMinDist)
		{
			// 각도 체크(카메라와 타겟이 이루는 각이 둔각일 경우 락온하지않는다.)
			_vector vCameraToTarget = XMVector3Normalize((XMVectorSetY(vTargetPos - vCameraPos, 0.f)));

			if (XMVector3Dot(vCameraLook, vCameraToTarget).m128_f32[0] <= 0.f)
				continue;

			fMinDist = fDist;
			pTarget = pGameObject;
		}
	}

	return pTarget;
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

	Safe_Release(m_pLockOnCurve);
}
