#pragma once

#include "VIBuffer.h"

/* ���� ���� �ټ� �����ϴ� Ŭ�������� �θ� ����ϴ� Ŭ���� */
/* �׷����� �ִ� ������ ���� ���º�ȯ�� ���(���)�ϴ� �������� ������ �ִ� ���������ϳ��� �߰������� �����Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance abstract : public CVIBuffer
{
protected:
	CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& rhs);
	virtual ~CVIBuffer_Instance() = default;

public:
	HRESULT Initialize_Prototype()	override;
	HRESULT Initialize(void* pArg)	override;
	HRESULT Render()				override;

protected:
	class CGameInstance*		m_pGameInstance = { nullptr };

protected:
	/* �߰��� ����(���)�� ������. */
	ID3D11Buffer*				m_pVBInstance = { nullptr };
	_uint						m_iNumInstance = { 0 };
	_uint						m_iInstanceVertexStride = { 0 };
	_uint						m_iNumIndexPerInstance = { 0 };

public:
	CComponent* Clone(void* pArg) = 0;
	void Free() override;
};

END