#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Client)

class CUrd_InversionSphere : public CGameObject
{
private:
	CUrd_InversionSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUrd_InversionSphere(const CUrd_InversionSphere& rhs);
	virtual ~CUrd_InversionSphere() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	_float		m_fScalingSpeed = { 20.f };
	_float3		m_vNowScale = { 1.f, 1.f, 1.f };
	_float		m_fTargetScale = { 250.f };

	_float		m_fLerpTime = { 0.7f };

private:
	CShader*	m_pShader = { nullptr };
	CModel*		m_pModel = { nullptr };

private:
	HRESULT Ready_Components(void* pArg);

public:
	static CUrd_InversionSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END

