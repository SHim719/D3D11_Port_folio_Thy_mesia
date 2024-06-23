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

protected:
	CMain_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMain_Camera(const CMain_Camera& rhs, CTransform::TRANSFORMDESC* pArg);
	virtual ~CMain_Camera() = default;

public:
	HRESULT Initialize(void* pArg)			override;
	void OnActive()							override;
	void OnInActive()						override;
	void Tick(_float fTimeDelta)			override;
	void LateTick(_float fTimeDelta)		override;

private:
	CameraState			m_eState = DEFAULT;

	class CPlayer*		m_pPlayer = { nullptr };
	CTransform*			m_pPlayerTransform = { nullptr };
	CTransform*			m_pTargetTransform = { nullptr };
	CBone*				m_pTargetBone = { nullptr };

	CBone*				m_pCutsceneBone = { nullptr };
	CTransform*			m_pCutsceneTargetTransform = { nullptr };
	_float4x4			m_CutsceneOffsetMatrix;
	
	_float4				m_vOffset = { 0.f, 2.0f, -2.8f, 0.f };
	_float4				m_vPrevTargetPos = {};

	_int				m_iSensitivity = 2;
	_float				m_fFollowingSpeed = 4.5f;
	_float				m_fLockonFollowingSpeed = 4.f;
public:
	void Set_Player(CGameObject* pPlayer);
	
	_bool SetState_LockOn();
	void SetState_Cutscene(const ATTACHDESC& Desc);

	void Reset_State();
private:
	void Default_State(_float fTimeDelta);
	void LockOn_State(_float fTimeDelta);
	void Cutscene_State(_float fTimeDelta);

private:
	_vector Get_LerpedTargetPos(_fvector vTargetPos, _float fTimeDelta);

	void Rotate_By_Mouse(_float fTimeDelta);
	void Follow_Target(_float fTimeDelta);
	CBone* Find_TargetBone(CModel* pModel);
	

public:
	static CMain_Camera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	void Free()	override;
};

END

