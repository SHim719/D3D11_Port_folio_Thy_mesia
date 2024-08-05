#pragma once

#include "Client_Defines.h"
#include "GameEffect.h"

BEGIN(Client)
class CEffect_Trail final : public CGameEffect
{
private:
	CEffect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Trail(const CEffect_Trail& rhs);
	virtual ~CEffect_Trail() = default;

public:
	HRESULT Initialize_Prototype(ifstream& fin);
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	HRESULT Load_EffectData(ifstream& fin)	override;

private:
	TRAIL_DESC			m_tTrailDesc = {};

	_float				m_fTrailUpdateTime = { 0.f };
	_float				m_fTrailUpdateAcc = { 0.f };

	_float4x4			m_ParentMatrix = {};

	_float4				m_vColor = { 1.f, 1.f, 1.f, 1.f };

public:
	void Reset_TrailAcc() {
		m_fTrailUpdateAcc = m_fTrailUpdateTime;
	}

	void Update_ParentMatrix(_matrix ParentMatrix) {
		XMStoreFloat4x4(&m_ParentMatrix, ParentMatrix);
	}

private:
	CVIBuffer_Trail*	m_pTrailBuffer = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_GlobalVariables()	override;
	HRESULT Bind_ShaderResources()	override;

public:
	static CEffect_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& fin);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;

	friend class CAnim_Tool;
};

END

