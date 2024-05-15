#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBackground final : public CGameObject
{
private:
	CBackground(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackground(const CBackground& rhs);
	virtual ~CBackground() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*		m_pShader		= nullptr;
	CTexture*		m_pTexture		= nullptr;
	CVIBuffer_Rect* m_pVIBuffer		= nullptr;

private:
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_ViewMatrix;
	_float4x4				m_ProjMatrix;

private:
	HRESULT Ready_Components();

public:
	static CBackground* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

