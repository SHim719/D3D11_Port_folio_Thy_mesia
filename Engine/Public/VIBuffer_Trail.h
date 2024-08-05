#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail final : public CVIBuffer
{
private:
	CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
	virtual ~CVIBuffer_Trail() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Update(_matrix ParentMatrix);

	HRESULT Init_Buffers();
	HRESULT Remake_Buffers(const TRAIL_DESC& TrailDesc);

	void Reset_Points();
private:
	TRAIL_DESC		m_tTrailDesc = {};

	_uint			m_iNowVertexCount = { 0 };
	_uint			m_iCatmullRomIndex[4] = {};
public:
	static CVIBuffer_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END