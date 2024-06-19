#pragma once

#include "Component.h"

BEGIN(Engine)

class CNavigation;

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

	typedef struct tagTransformDesc
	{
		_float			fSpeedPerSec;
		_float			fRotationPerSec;
	}TRANSFORMDESC;


private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState) const {
		return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);
	}

	_matrix Get_WorldMatrix() const {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	_float4x4 Get_WorldFloat4x4() const {
		return m_WorldMatrix;
	}

	void Set_WorldMatrix(_fmatrix WorldMatrix) {
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	}

	_float4x4 Get_WorldFloat4x4_TP() const {
		_float4x4	WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(Get_WorldMatrix()));
		return WorldMatrix;
	}

	_matrix Get_WorldMatrixInverse() const {
		return XMMatrixInverse(nullptr, Get_WorldMatrix());
	}

	void Set_State(STATE eState, _fvector vState);


	_vector Get_Position() const {
		return Get_State(STATE_POSITION);
	}

	void Set_Position(_fvector vPosition) {
		Set_State(STATE_POSITION, vPosition);
	}

	void Add_Position(_fvector vDir, _float fDist) {
		Set_Position(Get_Position() + vDir * fDist);
	}

	_vector Get_Right() const {
		return XMVector3Normalize(Get_State(STATE_RIGHT));
	}

	_vector Get_Up() const {
		return XMVector3Normalize(Get_State(STATE_UP));
	}

	_vector Get_Look() const {
		return XMVector3Normalize(Get_State(STATE_LOOK));
	}

	_vector Get_GroundRight() const {
		return XMVector3Normalize(XMVectorSetY(Get_Right(), 0.f));
	}

	_vector Get_GroundLook() const {
		return XMVector3Normalize(XMVectorSetY(Get_Look(), 0.f));
	}


public:
	HRESULT Initialize_Prototype()				override;
	HRESULT Initialize(void* pArg = nullptr)	override;

public:
	void Go_Straight(_float fTimeDelta, CNavigation* pNavigation = nullptr);
	void Go_Backward(_float fTimeDelta, CNavigation* pNavigation = nullptr);
	void Go_Left(_float fTimeDelta, CNavigation* pNavigation = nullptr);
	void Go_Right(_float fTimeDelta, CNavigation* pNavigation = nullptr);
	void Go_Up(_float fTimeDelta);
	void Go_Down(_float fTimeDelta);
	void Go_Dir(_fvector vDir, _float fTimeDelta, CNavigation* pNavigation = nullptr);
	void Go_Root(_fvector vDeltaRoot, _fvector vLook, CNavigation* pNavigation = nullptr);
	void Move_Root(_fvector vDeltaRoot, CNavigation* pNavigation = nullptr);

	void Set_Scale(_float3 vScale);
	_float3 Get_Scale() const;
	
	void Turn(_fvector vAxis, _float fTimeDelta);
	void Rotation(_fvector vAxis, _float fRadian);
	void Rotation_Quaternion(_fvector vQuat);
	void LookAt(_fvector vAt);
	void LookAt2D(_fvector vAt);
	void LookTo(_fvector vTo);


	void Set_Speed(_float fSpeed) { m_TransformDesc.fSpeedPerSec = fSpeed; }
	_float Get_Speed() const { return m_TransformDesc.fSpeedPerSec; }

	void Add_RollInput(_float fRadian);
	void Add_YAxisInput(_float fRadian);

	void Set_MoveLook(_fvector vLook) { XMStoreFloat4(&m_vMoveLook, vLook); }
	_vector Get_MoveLook() const { return XMLoadFloat4(&m_vMoveLook); }
	
	void Attach_To_Bone(class CBone* pBone, CTransform* pParentTransform, _fmatrix OffsetMatrix = XMMatrixIdentity(), _bool bOnlyPosition = false);

private:
	_float4x4				m_WorldMatrix;
	TRANSFORMDESC			m_TransformDesc; 
	_float4					m_vMoveLook;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent* Clone(void* pArg = nullptr) override;
	void Free() override;
};

END