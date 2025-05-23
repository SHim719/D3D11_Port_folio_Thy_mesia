#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point final : public CVIBuffer
{
private:
	CVIBuffer_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point(const CVIBuffer_Point& rhs);
	virtual ~CVIBuffer_Point() = default;

public:
	HRESULT Initialize_Prototype()	override;
	HRESULT Initialize(void* pArg)	override;
	HRESULT Bind_Buffers()			override;
	HRESULT Render()				override;


public:
	static CVIBuffer_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent* Clone(void* pArg)	override;
	void Free()	override;
};

END