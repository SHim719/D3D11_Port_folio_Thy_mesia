#include "Transform.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_TransformDesc{}
{

}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
	, m_TransformDesc{}
{

}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	_matrix WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	WorldMatrix.r[eState] = vState;

	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	return S_OK;
}

void CTransform::Go_Straight(_float fTimeDelta)
{
	_vector vPos = Get_Position();
	vPos += Get_Look() * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_Position(vPos);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector vPos = Get_Position();
	vPos -= Get_Look() * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_Position(vPos);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector vPos = Get_Position();
	vPos -= Get_Right() * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_Position(vPos);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector vPos = Get_Position();
	vPos += Get_Right() * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_Position(vPos);
}

void CTransform::Go_Up(_float fTimeDelta)
{
	_vector vPos = Get_Position();
	vPos += Get_Up() * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_Position(vPos);
}

void CTransform::Go_Down(_float fTimeDelta)
{
	_vector vPos = Get_Position();
	vPos -= Get_Up() * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_Position(vPos);
}

void CTransform::Set_Scale(_float3 vScale)
{
	Set_State(CTransform::STATE_RIGHT,
		XMVector3Normalize(Get_State(CTransform::STATE_RIGHT)) * vScale.x);
	Set_State(CTransform::STATE_UP,
		XMVector3Normalize(Get_State(CTransform::STATE_UP)) * vScale.y);
	Set_State(CTransform::STATE_LOOK,
		XMVector3Normalize(Get_State(CTransform::STATE_LOOK)) * vScale.z);

}

_float3 CTransform::Get_Scale() const 
{
	return _float3(XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT)))
		, XMVectorGetX(XMVector3Length(Get_State(STATE_UP)))
		, XMVectorGetX(XMVector3Length(Get_State(STATE_LOOK))));
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);
	
	_matrix	RotationMatrix = XMMatrixRotationAxis(vAxis, To_Radian(m_TransformDesc.fRotationPerSec) * fTimeDelta);

	vRight	= XMVector3TransformNormal(vRight, RotationMatrix);
	vUp		= XMVector3TransformNormal(vUp, RotationMatrix);
	vLook	= XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3		vScale = Get_Scale();
	
	_vector		vRight	=  XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector		vUp		=  XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector		vLook	=  XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix	RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	vRight	= XMVector3TransformNormal(vRight, RotationMatrix);
	vUp		= XMVector3TransformNormal(vUp, RotationMatrix);
	vLook	= XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::LookAt(_fvector vAt)
{
	_fvector		vLook	= vAt - Get_State(CTransform::STATE_POSITION);
	_fvector		vRight	= XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_fvector		vUp		= XMVector3Cross(vLook, vRight);

	_float3		vScale = Get_Scale();

	Set_State(CTransform::STATE_RIGHT,	XMVectorSetW(XMVector3Normalize(vRight)	 * vScale.x, 0.f));
	Set_State(CTransform::STATE_UP,		XMVectorSetW(XMVector3Normalize(vUp)	 * vScale.y, 0.f));
	Set_State(CTransform::STATE_LOOK,	XMVectorSetW(XMVector3Normalize(vLook)	 * vScale.z, 0.f));
}


void CTransform::Add_RollInput(_float fRadian)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp =	Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix	RotationMatrix = XMMatrixRotationAxis(vRight, fRadian);

	vRight	= XMVector3TransformNormal(vRight, RotationMatrix);
	vUp		= XMVector3TransformNormal(vUp,	RotationMatrix);
	vLook	= XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Add_YAxisInput(_float fRadian)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_vector		vYAxis = XMVectorSet(0.f, 1.0f, 0.f, 0.f);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vYAxis, fRadian);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTransform"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTransform"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CTransform::Free()
{
	__super::Free();
}