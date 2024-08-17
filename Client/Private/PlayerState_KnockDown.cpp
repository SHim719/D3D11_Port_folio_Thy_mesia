#include "PlayerState_KnockDown.h"


CPlayerState_KnockDown::CPlayerState_KnockDown(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_KnockDown::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayerState_KnockDown::OnState_Start(void* pArg)
{
	m_pPlayer->Set_Invincible(true);
	m_bCalcDuration = false;
	m_fTimeAcc = m_fDuration;

	m_pModel->Change_Animation(Corvus_SD_HurtKnockDown, 0.f, false);
}

void CPlayerState_KnockDown::Update(_float fTimeDelta)
{
	if (m_bCalcDuration)
	{
		m_fTimeAcc -= fTimeDelta;
		if (m_fTimeAcc < 0.f)
			m_fTimeAcc = 0.f;
	}
}

void CPlayerState_KnockDown::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete() && !m_bCalcDuration)
	{
		m_bCalcDuration = true;
	}
	else if (m_bCalcDuration && 0.f == m_fTimeAcc)
	{
		m_pPlayer->Change_State((_uint)PlayerState::State_KnockDown_GetUp);
	}

}

void CPlayerState_KnockDown::OnState_End()
{
	m_pPlayer->Set_Invincible(false);
}


CPlayerState_KnockDown* CPlayerState_KnockDown::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_KnockDown* pInstance = new CPlayerState_KnockDown(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_KnockDown"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_KnockDown::Free()
{
	__super::Free();
}
