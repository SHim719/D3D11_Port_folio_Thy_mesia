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

void CCharacter::Bind_KeyFrameSounds()
{
}

void CCharacter::Decide_PassIdx()
{
	if (m_bRimLight)
		m_iPassIdx = 3;
	else if (m_bAlphaBlend)
		m_iPassIdx = 1;
	else if (m_bDissolve)
		m_iPassIdx = 2;
	else
		m_iPassIdx = 0;
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

void CCharacter::OnDeath()
{
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

void CCharacter::Set_Active_AllWeapon_Colliders(_bool bActive)
{
	for (auto& pWeapon : m_Weapons)
	{
		pWeapon->Set_Active_Collider(false);
	}
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


void CCharacter::Update_RimLight(_float fTimeDelta)
{
	if (m_tRimLightDesc.fDuration < 0.f)
		return;

	m_fRimTimeAcc -= fTimeDelta;
	if (m_fRimTimeAcc < 0.f)
	{
		m_fRimTimeAcc = 0.f;
		Inactive_RimLight();
	}
	else
	{
		if (m_tRimLightDesc.bColorLerp)
			Lerp_RimColor();
	}
}

void CCharacter::Lerp_RimColor()
{
	_vector vLerpEndRimColor = XMLoadFloat4(&m_tRimLightDesc.vRimColor);
	_vector vLerpStartRimColor = XMVectorZero();

	_vector vLerpedRimColor = XMVectorLerp(vLerpStartRimColor, vLerpEndRimColor, m_fRimTimeAcc / m_tRimLightDesc.fDuration);

	XMStoreFloat4(&m_vNowRimColor, vLerpedRimColor);

}

HRESULT CCharacter::Bind_RimLightDescs()
{
	if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vRimColor", &m_vNowRimColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_fRimPower", &m_tRimLightDesc.fRimPower, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CCharacter::Active_RimLight(const RIMLIGHTDESC& RimDesc)
{
	m_bRimLight = true;
	m_tRimLightDesc = RimDesc;
	m_vNowRimColor = m_tRimLightDesc.vRimColor;
	m_fRimTimeAcc = RimDesc.fDuration;

	Decide_PassIdx();
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

	Safe_Release(m_pDissolveTexture);
}