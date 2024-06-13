#include "Enemy.h"


CEnemy::CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEnemy::CEnemy(const CEnemy& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnemy::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEnemy::Initialize(void* pArg)
{
	return S_OK;
}

void CEnemy::Tick(_float fTimeDelta)
{
}

void CEnemy::LateTick(_float fTimeDelta)
{

}

HRESULT CEnemy::Render()
{
	return S_OK;
}

void CEnemy::Free()
{
	__super::Free();

	Safe_Release(m_pModel);
	Safe_Release(m_pShader);
}
