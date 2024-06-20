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
	_bool		m_bPicked = { false };
	_bool		m_bUpdated = { false };

	_float		m_fScale = 0.3f;
	_float4		m_vColor = _float4(1.f, 0.f, 0.f, 1.f);


public:
	_bool Is_Picked() const {
		return m_bPicked;
	};

	_bool Check_Picked(_fvector vPoint);

	void Uncheck_Picked() {
		m_bPicked = false;
	}

	_vector Get_PointPos() const {
		return m_pTransform->Get_Position();
	}

	void Set_PointPos(_fvector vPointPos) {
		m_pTransform->Set_Position(vPointPos);
	}

	_bool Compare_PointPos(_fvector vSrc) const;

	_uint Get_RefCnt() const {
		return m_iRefCnt;
	}

	_bool Is_Updated() const {
		return m_bUpdated;
	}

	void Set_Update(_bool b) {
		m_bUpdated = b;
	}

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

