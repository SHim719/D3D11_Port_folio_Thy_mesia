#include "PlayerState_GetUp.h"


CPlayerState_GetUp::CPlayerState_GetUp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_GetUp::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayerState_GetUp::OnState_Start(void* pArg)
{
	m_pModel->Change_Animation(Corvus_SD_GetUp);
}

void CPlayerState_GetUp::Update(_float fTimeDelta)
{
}

void CPlayerState_GetUp::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
}

void CPlayerState_GetUp::OnState_End()
{
}

CPlayerState_GetUp* CPlayerState_GetUp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_GetUp* pInstance = new CPlayerState_GetUp(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_GetUp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_GetUp::Free()
{
	__super::Free();
}
