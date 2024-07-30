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

void CCharacter::Bind_KeyFrameEffects()
{
}

void CCharacter::Change_State(_uint eState, void* pArg)
{
	m_iPrevState = m_iState;
	
	m_States[m_iPrevState]->OnState_End();

	m_iState = eState;

	m_States[m_iState]->OnState_Start(pArg);
}

void CCharacter::Change_Navigation(CNavigation* pNavigation)
{
	for (auto& pState : m_States)
	{
		if (nullptr != pState)
			pState->Set_Navigation(pNavigation);
	}
		
}


void CCharacter::Hit(const ATTACKDESC& AttackDesc)
{
	m_States[m_iState]->OnHit(AttackDesc);
}

_int CCharacter::Take_Damage(const ATTACKDESC& AttackDesc)
{
	return 0;
}

void CCharacter::Tick_Weapons(_float fTimeDelta)
{
	for (auto& pWeapon : m_Weapons)
	{
		if (nullptr != pWeapon && pWeapon->Is_Active())
			pWeapon->Tick(fTimeDelta);
	}
}

void CCharacter::LateTick_Weapons(_float fTimeDelta)
{
	for (auto& pWeapon : m_Weapons)
	{
		if (nullptr != pWeapon && pWeapon->Is_Active())
			pWeapon->LateTick(fTimeDelta);
	}
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

void CCharacter::Update_Colliders()
{
	if (m_pCollider)
		m_pCollider->Update(m_pTransform->Get_WorldMatrix());

	if (m_pHitBoxCollider)
		m_pHitBoxCollider->Update(m_pTransform->Get_WorldMatrix());
}

CGameEffect::EFFECTSPAWNDESC CCharacter::Bake_EffectSpawnDesc()
{
	CGameEffect::EFFECTSPAWNDESC SpawnDesc;
	SpawnDesc.pParentModel = m_pModel;
	SpawnDesc.pParentTransform = m_pTransform;

	return SpawnDesc;
}


void CCharacter::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_States.size(); ++i)
		Safe_Release(m_States[i]);

	for (auto& pWeapon : m_Weapons)
		Safe_Release(pWeapon);
	m_Weapons.clear();

	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pCollider);
	Safe_Release(m_pHitBoxCollider);
	Safe_Release(m_pNavigation);

}