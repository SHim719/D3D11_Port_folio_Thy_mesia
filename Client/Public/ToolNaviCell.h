#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CToolNaviCell final : public CGameObject
{
private:
	CToolNaviCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolNaviCell(const CToolNaviCell& rhs);
	virtual ~CToolNaviCell() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;


private:
	class CToolNaviCellPoint*	m_pCellPoints[3] = {};
	size_t						m_iNumCellPoints = { 0 };

	_float4x4					m_IdentityMatrix;
	_float4						m_vColor = { 0.f, 1.f, 0.f, 1.f };

public:
	_bool Add_CellPoint(_fvector vPointPos);

private:
	CVIBuffer*	m_pVIBuffer = { nullptr };
	CShader*	m_pShader = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CToolNaviCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

