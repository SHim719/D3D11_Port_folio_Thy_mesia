#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CAisemy final : public CGameObject
{
private:
	CAisemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAisemy(const CAisemy& rhs);
	virtual ~CAisemy() = default;
	
private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	class CLightObject* m_pLight = { nullptr };

private:
	_float2		m_vNoiseUVOffset = { 0.f, 0.f };
	_float2		m_vNoiseUVSpeed = { 0.4f, -0.2f };

private:
	void Update_UV(_float fTimeDelta);

private:
	CTexture*	m_pBaseTexture = { nullptr };
	CTexture*	m_pNoiseTexture = { nullptr };

	CShader*	m_pShader = { nullptr };
	CModel*		m_pModel = { nullptr };

private:
	HRESULT Ready_Components(void* pArg);

	HRESULT Bind_GlobalVariables();
	HRESULT Bind_ShaderResources();

public:
	static CAisemy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END



