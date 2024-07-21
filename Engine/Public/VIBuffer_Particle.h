#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Particle final : public CVIBuffer_Instance
{
private:
	CVIBuffer_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Particle(const CVIBuffer_Particle& rhs);
	virtual ~CVIBuffer_Particle() = default;

public:
	HRESULT Initialize_Prototype()				override;
	HRESULT Initialize(void* pArg)				override;
	HRESULT Init_InstanceBuffer(_uint iNumInstance);
	void Update_Particle(const vector<VTXPARTICLE>& ParticleDatas);

	HRESULT Bind_Buffers()						override;
	HRESULT Render()							override;


public:
	static CVIBuffer_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent* Clone(void* pArg) override;
	void Free() override;
};

END