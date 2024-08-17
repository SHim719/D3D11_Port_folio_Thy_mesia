#include "PlayerState_KnockDown_GetUp.h"


CPlayerState_KnockDown_GetUp::CPlayerState_KnockDown_GetUp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_KnockDown_GetUp::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

void CPlayerState_KnockDown_GetUp::OnState_Start(void* pArg)
{
	m_pModel->Change_Animation(Corvus_SD_WeakGetUp_F);
}

void CPlayerState_KnockDown_GetUp::Update(_float fTimeDelta)
{
}

void CPlayerState_KnockDown_GetUp::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);

	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		Check_ExtraStateChange(ePlayerState);
}

void CPlayerState_KnockDown_GetUp::OnState_End()
{
}


CPlayerState_KnockDown_GetUp* CPlayerState_KnockDown_GetUp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_KnockDown_GetUp* pInstance = new CPlayerState_KnockDown_GetUp(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_KnockDown_GetUp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_KnockDown_GetUp::Free()
{
	__super::Free();
}
