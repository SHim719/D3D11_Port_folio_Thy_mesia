#include "Character.h"
#include "State_Base.h"

#include "Weapon.h"

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

_int CCharacter::Take_Damage(const ATTACKDESC& AttackDesc)
{
	return 0;
}

void CCharacter::Update_AttackDesc()
{
	pair<_uint, ATTACKDESC> AtkDesc = m_States[m_iState]->Get_NowAttackDesc();

	m_Weapons[AtkDesc.first]->Set_AttackDesc(AtkDesc.second);
}

void CCharacter::Set_Active_Weapon(_uint iWeaponIdx, _bool bActive)
{
	m_Weapons[iWeaponIdx]->Set_Active(bActive);
}

void CCharacter::Compute_YPos()
{
	_vector vNowPosition = m_pTransform->Get_Position();
	_float fY = m_pNavigation->Decide_YPos(vNowPosition);
	m_pTransform->Set_Position(XMVectorSetY(vNowPosition, fY));
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
	Safe_Release(m_pNavigation);
}