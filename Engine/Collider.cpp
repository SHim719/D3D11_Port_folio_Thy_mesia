#include "Collider.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameInstance.h"

_uint CCollider::iCollisionID = 0;

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent{ pGraphic_Device }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent{ rhs }
	, m_pGameInstance{ rhs.m_pGameInstance }
	, m_iCollisionID{ iCollisionID++ }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCollider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	return S_OK;
}

void CCollider::Render()
{
}

void CCollider::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
