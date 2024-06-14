#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CBone;
END

BEGIN(Client)

class CWeapon final : public CGameObject
{
public:
	typedef struct tagWeaponDesc
	{
		CBone* pSocketBone = nullptr;
		CTransform* pParentTransform = nullptr;
		wstring wstrModelTag = L"";
		CCollider::COLLIDERDESC* pColliderDesc = nullptr;
	} WEAPONDESC;

private:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

public:
	void Active_Collider() {
		m_pCollider->Set_Active(true);
	}
	void Inactive_Collider() {
		m_pCollider->Set_Active(false);
	}

private:
	CShader*		m_pShader = nullptr;
	CModel*			m_pModel = nullptr;
	CCollider*		m_pCollider = nullptr;

private:
	WEAPONDESC		m_WeaponDesc;

private:
	HRESULT Ready_Components();

public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END