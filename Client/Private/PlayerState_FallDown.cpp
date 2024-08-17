#include "PlayerState_FallDown.h"


CPlayerState_FallDown::CPlayerState_FallDown(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_FallDown::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayerState_FallDown::OnState_Start(void* pArg)
{
	m_pPlayer->Set_Adjust_NaviY(false);
	m_pPlayer->Set_Invincible(true);
	m_pModel->Change_Animation(Corvus_SD_HurtFallDown, 0.f, true);
}

void CPlayerState_FallDown::Update(_float fTimeDelta)
{
}

void CPlayerState_FallDown::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_GetUp);
}

void CPlayerState_FallDown::OnState_End()
{
	m_pPlayer->Set_Adjust_NaviY(true);
	m_pPlayer->Set_Invincible(false);
}


CPlayerState_FallDown* CPlayerState_FallDown::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_FallDown* pInstance = new CPlayerState_FallDown(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_FallDown"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_FallDown::Free()
{
	__super::Free();
}
