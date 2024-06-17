#include "Character.h"
#include "State_Base.h"

CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCharacter::CCharacter(const CCharacter& rhs)
	: CGameObject(rhs)
{
}

void CCharacter::Bind_KeyFrames()
{
}

void CCharacter::Change_State(_uint eState, void* pArg)
{
	m_iPrevState = m_iState;

	m_States[m_iPrevState]->OnState_End();

	m_iState = eState;

	m_States[m_iState]->OnState_Start(pArg);
}

void CCharacter::Hit(void* pArg)
{
}

void CCharacter::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_States.size(); ++i)
		Safe_Release(m_States[i]);

	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pCollider);
	Safe_Release(m_pHitBoxCollider);
}

const ATTACKDESC& CCharacter::Get_NowAttackDesc(size_t iIdx)
{
	return m_States[m_iState]->Get_NowAttackDesc(iIdx);
}
