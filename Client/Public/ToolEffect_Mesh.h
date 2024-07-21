#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CToolEffect_Mesh final : public CGameObject
{
private:
	CToolEffect_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolEffect_Mesh(const CToolEffect_Mesh& rhs);
	virtual ~CToolEffect_Mesh() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;

	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;

	HRESULT Render()					override;

	void Init_Attributes();

	void Update_Rotation(_float fTimeDelta);
	void Update_Scale(_float fTimeDelta);
	void Update_UVMask(_float fTimeDelta);
	void Update_UVNoise(_float fTimeDelta);

private:
	_int		m_iDiffuseTextureIdx = { 0 };
	_int		m_iMaskTextureIdx = { 0 };
	_int		m_iNoiseTextureIdx = { 0 };
	_int		m_iDissolveTextureIdx = { 0 };
	_int		m_iPassIdx = { 0 };

	_float4		m_vColor = { 1.f, 1.f, 1.f, 1.f };
	_float		m_fAlpha = { 1.f };
	_float		m_fLifeTime = { 1.f };

	//_float		m_fUVMaskSpeed = { 0.f };
	_float2		m_vUVMaskSpeed = { 0.f, 0.f };
	_float2		m_vUVMask = { 0.f, 0.f };

	//_float		m_fUVNoiseSpeed = { 0.f };
	_float2		m_vUVNoiseSpeed = { 0.f, 0.f };
	_float2		m_vUVNoise = { 0.f, 0.f };

	_float3		m_vRotationSpeed = { 0.f, 0.f, 0.f };
	_float3		m_vScaleSpeed = { 0.f, 0.f, 0.f };
private:
	CModel*				m_pModel = { nullptr };
	CTexture*			m_pDiffuseTexture = { nullptr };
	CTexture*			m_pMaskTexture = { nullptr };
	CTexture*			m_pNoiseTexture = { nullptr };
	CTexture*			m_pDissolveTexture = { nullptr };
	CShader*			m_pShader = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResoruces();

public:
	static CToolEffect_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;


	friend class CEffect_Tool;
};

END

