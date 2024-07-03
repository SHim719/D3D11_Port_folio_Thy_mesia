#include "PlayerState_ParrySuccess.h"

CPlayerState_ParrySuccess::CPlayerState_ParrySuccess(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_ParrySuccess::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Jog, PlayerState::State_LockOn
		, PlayerState::State_Attack, PlayerState::State_PlagueAttack, PlayerState::State_ChargeStart,
		PlayerState::State_Avoid, PlayerState::State_Parry };


    return S_OK;
}

void CPlayerState_ParrySuccess::OnState_Start(void* pArg)
{
	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_CanRotation(false);
	m_pPlayer->Set_Invincible(true);

	_int* iParryDir = (_int*)pArg;
	_int iRandNum = JoRandom::Random_Int(0, 1);

	if (0 == *iParryDir)
		m_pModel->Change_Animation(Corvus_SD_ParryDeflect_L + iRandNum, 0.f);
	else
		m_pModel->Change_Animation(Corvus_SD_ParryDeflect_R + iRandNum, 0.f);

	
}

void CPlayerState_ParrySuccess::Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);

	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);

}

void CPlayerState_ParrySuccess::Late_Update(_float fTimeDelta)
{
	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		Check_ExtraStateChange(ePlayerState);
}

void CPlayerState_ParrySuccess::OnState_End()
{
}

void CPlayerState_ParrySuccess::OnHit(const ATTACKDESC& AttackDesc)
{
	// 公利惑怕 贸府

	if (!m_pPlayer->Is_Invincible())
		int x = 10;
}

CPlayerState_ParrySuccess* CPlayerState_ParrySuccess::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_ParrySuccess* pInstance = new CPlayerState_ParrySuccess(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_ParrySuccess"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ParrySuccess::Free()
{
	__super::Free();
}
