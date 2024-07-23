#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CArchive_Chair final : public CGameObject
{
private:
	CArchive_Chair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CArchive_Chair(const CArchive_Chair& rhs);
	virtual ~CArchive_Chair() = default;

	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

public:
	void Active_Chair();

private:
	_bool m_bFound = { false };

private:
	void OnCollisionStay(CGameObject* pOther)	override;

private:
	CCollider*	m_pCollider = { nullptr };
	CShader*	m_pShader = { nullptr };
	CModel*		m_pModel = { nullptr };

private:
	HRESULT Ready_Components(void* pArg);

public:
	static CArchive_Chair* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END



