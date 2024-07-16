#include "PlayerState_FoundBeacon.h"

#include "Archive_Chair.h"


CPlayerState_FoundBeacon::CPlayerState_FoundBeacon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_FoundBeacon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	m_pModel->Bind_Func("Active_Chair", bind(&CPlayerState_FoundBeacon::Active_Chair, this));

	return S_OK;
}

void CPlayerState_FoundBeacon::OnState_Start(void* pArg)
{
	m_pActivating_Chair = (CArchive_Chair*)pArg;
	Safe_AddRef(m_pActivating_Chair);

	m_pPlayer->Set_Adjust_NaviY(true);

	m_pModel->Change_Animation(Corvus_ArchiveLight_up);
}

void CPlayerState_FoundBeacon::Update(_float fTimeDelta)
{
}

void CPlayerState_FoundBeacon::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
}

void CPlayerState_FoundBeacon::OnState_End()
{
	m_pPlayer->Set_Adjust_NaviY(false);

	Safe_Release(m_pActivating_Chair);
}

void CPlayerState_FoundBeacon::Active_Chair()
{
	m_pActivating_Chair->Active_Chair();
}


CPlayerState_FoundBeacon* CPlayerState_FoundBeacon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_FoundBeacon* pInstance = new CPlayerState_FoundBeacon(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_FoundBeacon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_FoundBeacon::Free()
{
	__super::Free();

	Safe_Release(m_pActivating_Chair);
}
