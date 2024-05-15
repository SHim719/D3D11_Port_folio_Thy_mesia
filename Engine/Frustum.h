#pragma once
#include "Base.h"


class CFrustum : public CBase
{
private:
	//CFrustum(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Tick();

	_bool In_WorldFrustum(_float3 vWorldPos, _float fRadius);

private:
	//LPDIRECT3DDEVICE9 m_pGraphic_Device = { nullptr };

	_float3 m_vProjFrustumPoints[8];
	_float3 m_vWorldFrustumPoints[8];

	//D3DXPLANE m_WorldFrustumPlane[6];
public:
	//static CFrustum* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

