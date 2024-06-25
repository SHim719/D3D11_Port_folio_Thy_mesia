#include "PlayerState_Attack.h"


CPlayerState_Attack::CPlayerState_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Attack::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModel->Bind_Func("Enable_NextAttack", bind(&CPlayerState_Attack::Set_CanNextAttack, this, true));
	m_pModel->Bind_Func("Disable_NextAttack", bind(&CPlayerState_Attack::Set_CanNextAttack, this, false));

	m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_Avoid, PlayerState::State_Parry };

	return S_OK;
}

void CPlayerState_Attack::OnState_Start(void* pArg)
{
	m_bCanNextAttack = false;
	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_CanRotation(true);

	Reset_AttackIdx();

	m_pModel->Change_Animation(_uint(Corvus_SD_LAttack1 + m_iNowComboCnt), 0.f);
}

void CPlayerState_Attack::OnGoing(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
		return;
	}

	if (m_pPlayer->Can_Rotation() && false == m_pPlayer->Is_LockOn())
	{
		_vector vNewLook = Calc_MoveLook(true);

		if (0.f != vNewLook.m128_f32[0] || 0.f != vNewLook.m128_f32[1])
			Rotate_To_Look(vNewLook, fTimeDelta);
	}

	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);

	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
	{
		if (PlayerState::State_Attack == ePlayerState)
		{
			if (true == m_bCanNextAttack && m_iNowComboCnt < m_pPlayerStats->Get_MaxAttackCnt() - 1)
			{
				++m_iNowComboCnt;
				OnState_Start(nullptr);
			}
		}
		else
		{
			m_pPlayer->Change_State((_uint)ePlayerState);
		}
	}
		
}

void CPlayerState_Attack::OnState_End()
{
	m_iNowComboCnt = 0;
}

void CPlayerState_Attack::Init_AttackDesc()
{
	m_AttackDescs.reserve(1);
	m_AttackDescs.emplace_back(CPlayer::SABER , ATTACKDESC());
}


CPlayerState_Attack* CPlayerState_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Attack* pInstance = new CPlayerState_Attack(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Attack::Free()
{
	__super::Free();
}
