#include "PlayerState_Healing.h"


CPlayerState_Healing::CPlayerState_Healing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Healing::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayerState_Healing::OnState_Start(void* pArg)
{
	m_pModel->Change_Animation(Corvus_Healing2_InPlace);
}

void CPlayerState_Healing::Update(_float fTimeDelta)
{

}

void CPlayerState_Healing::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
	}
}

void CPlayerState_Healing::OnState_End()
{
}


CPlayerState_Healing* CPlayerState_Healing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Healing* pInstance = new CPlayerState_Healing(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Healing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Healing::Free()
{
	__super::Free();
}
