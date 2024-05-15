#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CCollider;
class CRenderer;
class CTransform;
class CNavigation;


class CHierarchyNode;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum PARTTYPE { PART_BODY, PART_HEAD, PART_REGIONARM, PART_END };
//	enum COLLIDERTYPE { COLLIDERTYPE_AABB, COLLIDERTYPE_OBB, COLLIDERTYPE_SPHERE, COLLILDERTYPE_END };
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	CShader*	m_pShader = nullptr;
	CModel*		m_pModels[PARTTYPE::PART_END] = {};
	//CCollider* m_pColliderCom[COLLILDERTYPE_END] = { nullptr };
	//CNavigation* m_pNavigationCom = nullptr;

private:
	vector<CGameObject*>				m_Parts;
	typedef vector<CGameObject*>		PARTS;

	//vector<class CHierarchyNode*>		m_Sockets;

private:
	HRESULT Ready_Components();

	//HRESULT Update_Weapon();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END