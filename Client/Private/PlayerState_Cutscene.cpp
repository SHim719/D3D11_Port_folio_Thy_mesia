#include "PlayerState_Cutscene.h"

CPlayerState_Cutscene::CPlayerState_Cutscene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Cutscene::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_Avoid, PlayerState::State_Parry };

	return S_OK;
}

void CPlayerState_Cutscene::OnState_Start(void* pArg)
{
	m_pPlayer->Disable_NextState();
	m_pPlayer->Disable_Rotation();
	m_pPlayer->Inactive_AllWeaponColliders();

	CUTSCENE_NUMBER* pCutsceneNum = (CUTSCENE_NUMBER*)pArg;

	//switch (*pCutsceneNum)
	//{
	//case ENCOUNTER_ODUR:
	//	m_pModel->Change_Animation(Corvus_VS_MagicianLV1_Seq_BossFightStart, 0.f);
	//	break;
	//case ENCOUNTER_URD:
	//	break;
	//}

	m_pModel->Change_Animation(Corvus_VS_MagicianLV1_Seq_BossFightStart, 0.f);
}

void CPlayerState_Cutscene::OnGoing(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
	}
}

void CPlayerState_Cutscene::OnState_End()
{

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
