#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CLockOnCurve final : public CGameObject
{
public:
	typedef struct tagCurveDesc
	{
		_float4x4 PlayerWorldMatrix = {};
		_float4 vTargetPos = {};
	}CURVE_DESCS;

private:
	CLockOnCurve(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLockOnCurve(const CLockOnCurve& rhs);
	virtual ~CLockOnCurve() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;
	
	void Set_CurveDescs(const CURVE_DESCS& CurveDescs) {
		m_tCurveDescs = CurveDescs;
	}

private:
	CURVE_DESCS		m_tCurveDescs = {};
	_float2			m_vMaskUVOffset = { 1.f, 0.f };

private:
	CShader*			m_pShader = { nullptr };
	CVIBuffer_Point*	m_pVIBuffer = { nullptr };
	CTexture*			m_pMaskTexture = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CLockOnCurve* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END