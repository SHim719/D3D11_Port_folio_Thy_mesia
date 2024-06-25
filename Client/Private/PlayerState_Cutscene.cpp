#include "PlayerState_Cutscene.h"

CPlayerState_Cutscene::CPlayerState_Cutscene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Cutscene::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayerState_Cutscene::OnState_Start(void* pArg)
{
	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_CanRotation(false);
	m_pPlayer->Inactive_AllWeaponColliders();
	m_pPlayer->Set_Active_Colliders(false);
	m_pPlayer->Set_NoRender(true);

	CUTSCENE_NUMBER* pCutsceneNum = (CUTSCENE_NUMBER*)pArg;

	switch (*pCutsceneNum)
	{
	case ENCOUNTER_ODUR:
		m_pModel->Change_Animation(Corvus_VS_MagicianLV1_Seq_BossFightStart, 0.f);
		break;
	case ENCOUNTER_URD:
		break;
	}

}

void CPlayerState_Cutscene::OnGoing(_float fTimeDelta)
{
}

void CPlayerState_Cutscene::OnState_End()
{
	m_pPlayer->Set_Active_Colliders(true);
	m_pMain_Camera->Reset_State();
}

CPlayerState_Cutscene* CPlayerState_Cutscene::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Cutscene* pInstance = new CPlayerState_Cutscene(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Cutscene"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Cutscene::Free()
{
	__super::Free();
}
