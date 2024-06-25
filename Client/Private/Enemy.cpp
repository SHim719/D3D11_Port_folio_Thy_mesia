#include "Enemy.h"
#include "State_Base.h"

CGameObject* CEnemy::s_pTarget = nullptr;

CEnemy::CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter(pDevice, pContext)
{
}

CEnemy::CEnemy(const CEnemy& rhs)
	: CCharacter(rhs)
{
}


void CEnemy::OnCollisionEnter(CGameObject* pOther)
{
	if (TAG_PLAYER == pOther->Get_Tag())
		m_bCollPlayer = true;
}

void CEnemy::OnCollisionExit(CGameObject* pOther)
{
	if (TAG_PLAYER == pOther->Get_Tag())
		m_bCollPlayer = false;
}

void CEnemy::Free()
{
	__super::Free();

	Safe_Release(m_pStats);
}
