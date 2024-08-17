#include "PlayerState_Parry.h"
#include "PlayerState_ParrySuccess.h"

CPlayerState_Parry::CPlayerState_Parry(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Parry::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Jog, PlayerState::State_LockOn
		, PlayerState::State_Attack, PlayerState::State_PlagueAttack, PlayerState::State_ChargeStart,
		PlayerState::State_Avoid, PlayerState::State_Parry, PlayerState::State_Healing };

	return S_OK;
}

void CPlayerState_Parry::OnState_Start(void* pArg)
{
	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_CanRotation(true);
	m_pPlayer->Set_EnableJog(false);
	m_pPlayer->Set_CanParry(true);

	if ((_uint)PlayerState::State_ParrySuccess == m_pPlayer->Get_PrevState() ||
		(_uint)PlayerState::State_Parry == m_pPlayer->Get_PrevState())
		m_iParryDir = (m_iParryDir + 1) % 2;
	else
		m_iParryDir = 0;

	if (false == m_pPlayer->Is_LockOn())
	{
		_vector vMoveLook = Calc_MoveLook(true);

		if (0.f != vMoveLook.m128_f32[0] || 0.f != vMoveLook.m128_f32[1])
			m_pOwnerTransform->LookTo(vMoveLook);
	}

	m_pModel->Change_Animation(Corvus_SD1_ParryL_NEW + m_iParryDir, 0.f);
}

void CPlayerState_Parry::Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
}

void CPlayerState_Parry::Late_Update(_float fTimeDelta)
{
	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		Check_ExtraStateChange(ePlayerState);
}

void CPlayerState_Parry::OnState_End()
{
	m_pPlayer->Set_EnableJog(true);
}

void CPlayerState_Parry::OnHit(const ATTACKDESC& AttackDesc)
{
	if (true == m_pPlayer->Can_Parry() && AttackDesc.eEnemyAttackType < AIR)
	{
		CPlayerState_ParrySuccess::PARRY_DESC ParryDesc{};
		ParryDesc.iParryDir = m_iParryDir;
		ParryDesc.AttackDesc = AttackDesc;
		m_pPlayer->Change_State((_uint)PlayerState::State_ParrySuccess, &ParryDesc);
	
	}
	else
	{
		__super::OnHit(AttackDesc);
	}
}

CPlayerState_Parry* CPlayerState_Parry::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Parry* pInstance = new CPlayerState_Parry(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Parry"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Parry::Free()
{
	__super::Free();
}
