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

	_bool Intersect_Ray(_fvector vRayStartPos, _fvector vRayDir);
	void Uncheck_Picking();

private:
	vector<class CToolNaviCellPoint*>	m_CellPoints;

	_float4x4							m_IdentityMatrix;
	_float4								m_vColor = { 0.f, 1.f, 0.f, 1.f };

	_bool								m_bPicked = { false };

	_int								m_iCellAttribute = { 0 };
private:
	CVIBuffer*	m_pVIBuffer = { nullptr };
	CShader*	m_pShader = { nullptr };

public:
	_float3* Get_CellPoints();
	void Set_CellPoints(_float3* vPoints);
	void Remake_VIBuffer();

	_int Get_Attribute() const { return m_iCellAttribute; }
	void Set_Attribute(_int iAttribute) { m_iCellAttribute = iAttribute; }

private:
	HRESULT Ready_Components();

public:
	static CToolNaviCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

