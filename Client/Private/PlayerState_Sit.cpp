#include "PlayerState_Sit.h"


CPlayerState_Sit::CPlayerState_Sit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Sit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayerState_Sit::OnState_Start(void* pArg)
{
	m_pMain_Camera->SetState_UI();
	m_pPlayerStats->SetHp_Full();
	m_pPlayerStats->Add_PotionCount(5);

	m_pModel->Change_Animation(Corvus_ArchiveSitStart);
}

void CPlayerState_Sit::Update(_float fTimeDelta)
{
}

void CPlayerState_Sit::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		switch (m_pModel->Get_CurrentAnimIndex())
		{
		case Corvus_ArchiveGetup:
			m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
			m_pMain_Camera->SetState_Default();
			break;
		case Corvus_ArchiveSitStart:
			m_pModel->Change_Animation(Corvus_ArchiveSitLoop);
			break;
		}
	}	
}

void CPlayerState_Sit::OnState_End()
{
}


CPlayerState_Sit* CPlayerState_Sit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Sit* pInstance = new CPlayerState_Sit(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Sit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Sit::Free()
{
	__super::Free();
}
