#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CLadder_Up final : public CGameObject
{
private:
	CLadder_Up(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLadder_Up(const CLadder_Up& rhs);
	virtual ~CLadder_Up() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	void OnCollisionStay(CGameObject* pOther)	override;

private:
	LADDERDESC	m_LadderDesc;

	CCollider*	m_pCollider = { nullptr };
	CShader*	m_pShader = { nullptr };
	CModel*		m_pModel = { nullptr };

private:
	HRESULT Ready_Components(void* pArg);

public:
	static CLadder_Up* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END

