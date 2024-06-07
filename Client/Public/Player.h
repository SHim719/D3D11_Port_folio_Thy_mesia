#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Player_Defines.h"

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
	CModel*		m_pModel = nullptr;
	//CCollider* m_pColliderCom[COLLILDERTYPE_END] = { nullptr };
	//CNavigation* m_pNavigationCom = nullptr;

private:
	PlayerState			m_eState = PlayerState::State_End;
	class CState_Base*	m_States[(_uint)PlayerState::State_End] = {};

public:
	void Change_State(PlayerState eState);

private:
	HRESULT Ready_Components();
	HRESULT Ready_States();
	HRESULT Ready_Weapons();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END