#pragma once

#include "Client_Defines.h"
#include "GameEffect.h"

BEGIN(Client)
class CToolEffect_Trail final : public CGameEffect
{
private:
	CToolEffect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolEffect_Trail(const CToolEffect_Trail& rhs);
	virtual ~CToolEffect_Trail() = default;

private:
	HRESULT Initialize_Prototype(const wstring& wstrFilePath);
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	HRESULT Save_EffectData(ofstream& fout)	override;
	HRESULT Load_EffectData(ifstream& fin)	override;

private:
	void Update_Buffer();
	void Reset_Points();

private:
	_float4				m_vColor = { 1.f, 1.f, 1.f, 1.f };

	_float				m_fTrailUpdateTime = { 0.f };
	_float				m_fTrailUpdateAcc = { 0.f };
	_float3				m_vStartPos = {};
	_float3				m_vEndPos = {};
	_uint				m_iMaxVertexCount = { 100 };
	_uint				m_iCatmullRomCount = { 12 };
	_uint				m_iRemoveCount = { 0 };

	_float4x4			m_ParentMatrix = {};

private:
	CVIBuffer_Trail*	m_pTrailBuffer = { nullptr };


private:
	HRESULT Ready_Components();
	HRESULT Bind_GlobalVariables()	override;
	HRESULT Bind_ShaderResources()	override;

public:
	static CToolEffect_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CToolEffect_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& wstrFilePath);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;

	friend class CAnim_Tool;
};

END




