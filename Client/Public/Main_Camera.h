#pragma once

#include "Client_Defines.h"
#include "MyCamera.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class CMain_Camera final : public CCamera
{
public:
	enum CameraState { DEFAULT, LOCKON, CUTSCENE, UI,TARGET_END };

	typedef struct tagShakingDesc
	{
		_float fShakeTime = 0.f;
		_float fShakingForce = 0.f;
	}SHAKINGDESC;

	typedef struct tagDeltaFovY
	{
		_float fTargetFovY = 0.f;
		_float fTargetLerpTime = 0.f;
		_float fReturnLerpTime = 0.f;
	}DELTAFOVYDESC;

protected:
	CMain_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMain_Camera(const CMain_Camera& rhs, CTransform::TRANSFORMDESC* pArg);
	virtual ~CMain_Camera() = default;

public:
	HRESULT Initialize(void* pArg)			override;
	void Init_CameraShakingDescs();

	void OnActive()							override;

	void PriorityTick(_float fTimeDelta)	override;
	void Tick(_float fTimeDelta)			override;
	void LateTick(_float fTimeDelta)		override;

private:
	CameraState			m_eState = DEFAULT;

	class CPlayer*		m_pPlayer = { nullptr };
	CTransform*			m_pPlayerTransform = { nullptr };

	class CEnemy*		m_pTarget = { nullptr };
	CTransform*			m_pTargetTransform = { nullptr };
	CBone*				m_pTargetBone = { nullptr };

	CBone*				m_pCutsceneBone = { nullptr };
	CTransform*			m_pCutsceneTargetTransform = { nullptr };
	_float4x4			m_CutsceneOffsetMatrix;
	
	_float4				m_vOffset = { 0.f, 1.5f, -3.25f, 0.f };//{ 0.f, 2.0f, -2.8f, 0.f };
	_float4				m_vPrevTargetPos = {};

	_int				m_iSensitivity = 2;
	_float				m_fFollowingSpeed = 4.5f;
	_float				m_fLockonFollowingSpeed = 4.f;

private:
	unordered_map<string, SHAKINGDESC>	m_ShakingDescs;
	SHAKINGDESC							m_tShakingDesc = {};
	_float4								m_vShakingOffset = {};
	_bool								m_bShaking = { false };

private:
	DELTAFOVYDESC						m_tDeltaFovYDesc = {};
	_bool		 						m_bDeltaFovY = { false };
	_bool		 						m_bIncreaseFovY = { true };
	_float		 						m_fOriginFov = { 70.f};
	_float								m_fDeltaFovY_TimeAcc = { 0.f };

private:
	class CLockOnCurve* m_pLockOnCurve = { nullptr };

public:
	void Set_Player(CGameObject* pPlayer);
	
	void SetState_Default();
	void SetState_LockOn();
	void SetState_LockOn_To_Default();
	void SetState_UI();

	void SetState_Cutscene(const ATTACHDESC& Desc);
	void Reset_CutsceneState();

	void Play_CameraShake(const string& strTag);
	void Add_DeltaFovYDesc(const DELTAFOVYDESC& DeltaFovYDesc);
	void End_DeltaFov(_float fReturnLerpTime);

private:
	void Default_State(_float fTimeDelta);
	void LockOn_State(_float fTimeDelta);
	void Cutscene_State(_float fTimeDelta);

	void Shaking(_float fTimeDelta);
	void Delta_FovY(_float fTimeDelta);

private:
	_vector Get_LerpedTargetPos(_fvector vTargetPos, _float fTimeDelta);

	void Rotate_By_Mouse(_float fTimeDelta);
	void Follow_Target(_float fTimeDelta);
	CGameObject* Find_LockOnTarget();
	CBone* Find_TargetBone(CModel* pModel);
	void Update_LockOnCurveDesc();
	
public:
	static CMain_Camera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	void Free()	override;
};

END

