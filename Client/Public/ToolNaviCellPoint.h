#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CToolNaviCellPoint final : public CGameObject
{
private:
	CToolNaviCellPoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolNaviCellPoint(const CToolNaviCellPoint& rhs);
	virtual ~CToolNaviCellPoint() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	_bool m_bPicked = { false };

public:
	_bool Is_Picked() const {
		return m_bPicked;
	};

	_bool Check_Picked() const {

	}

	_vector Get_PointPos() const {
		return m_pTransform->Get_Position();
	}

private:
	_float4 m_vColor = _float4(1.f, 0.f, 0.f, 1.f);

private:
	CVIBuffer*	m_pVIBuffer = { nullptr };
	CShader*	m_pShader = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CToolNaviCellPoint* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

