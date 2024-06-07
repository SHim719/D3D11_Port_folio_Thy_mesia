#pragma once


#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Client)
class CToolAnimObj final : public CGameObject
{
private:
	CToolAnimObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolAnimObj(const CToolAnimObj& rhs);
	virtual ~CToolAnimObj() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	CShader*	m_pShader = nullptr;
	CModel*		m_pModel = nullptr;

private:
	HRESULT Ready_Components(void* pArg);

public:
	CModel* Get_Model() const { return m_pModel; }

public:
	static CToolAnimObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

