#include "PlayerState_Parry.h"

CPlayerState_Parry::CPlayerState_Parry(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Parry::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_Avoid, PlayerState::State_Parry };

	m_pModel->Bind_Func("Enable_Parry", bind(&CPlayerState_Parry::Enable_Parry, this));
	m_pModel->Bind_Func("Disable_Parry", bind(&CPlayerState_Parry::Disable_Parry, this));

	return S_OK;
}

void CPlayerState_Parry::OnState_Start(void* pArg)
{
	m_pPlayer->Disable_NextState();
	m_pPlayer->Enable_Rotation();
	m_bCanParry = true;

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

	m_pModel->Change_Animation(Corvus_SD1_ParryL_NEW + m_iParryDir);
}

void CPlayerState_Parry::OnGoing(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);

	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		m_pPlayer->Change_State((_uint)ePlayerState);
}


void CPlayerState_Parry::OnState_End()
{
	
}

void CPlayerState_Parry::OnHit(void* pArg)
{
	if (m_bCanParry)
		m_pPlayer->Change_State((_uint)PlayerState::State_ParrySuccess, &m_iParryDir);

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
