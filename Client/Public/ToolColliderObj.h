#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Client)
class CToolColliderObj final : public CGameObject
{
private:
	CToolColliderObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolColliderObj(const CToolColliderObj& rhs);
	virtual ~CToolColliderObj() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	CGameObject*	m_pOwner = nullptr;
	CCollider*		m_pCollider = nullptr;

	CModel*					m_pOwnerModel = nullptr;
	class Engine::CBone*	m_pAttachBone = nullptr;
public:
	void Set_AttachBone(const _char* szBoneName);
	void Remake_Collider(CCollider::COLLIDERDESC* pDesc) {
		m_pCollider->Remake_Collider(pDesc);}

public:
	static CToolColliderObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

