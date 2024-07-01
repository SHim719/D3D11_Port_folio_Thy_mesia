#include "PlayerState_Execution_Default.h"

#include "Enemy.h"

CPlayerState_Execution_Default::CPlayerState_Execution_Default(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Execution_Default::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Execution_Default, PlayerState::State_ChargeStart, 
		PlayerState::State_Avoid, PlayerState::State_Parry };

	return S_OK;
}

void CPlayerState_Execution_Default::OnState_Start(void* pArg)
{
	m_pPlayer->Set_CanNextState(false);

	m_pExecutionEnemy = (CEnemy*)pArg;
	m_pExecutionEnemy->Set_Active_Colliders(false);

	if (false == m_pPlayer->Is_LockOn())
		m_pOwnerTransform->LookAt2D(m_pExecutionEnemy->Get_Transform()->Get_Position());

	m_pOwnerTransform->Set_Speed(m_fRushSpeed);

	XMStoreFloat4(&m_vStartPos, m_pOwnerTransform->Get_Position());

	m_pModel->Change_Animation(Corvus_StunExecute_StartRDown_L + m_iExecutionCount % 2);
}

void CPlayerState_Execution_Default::Update(_float fTimeDelta)
{
	if (nullptr != m_pExecutionEnemy)
	{
		_float fDist = XMVector3Length(m_pExecutionEnemy->Get_Transform()->Get_Position() - m_pOwnerTransform->Get_Position()).m128_f32[0];
		if (fDist < 1.5f)
		{
			m_pExecutionEnemy->SetState_Death();
			m_pExecutionEnemy = nullptr;
		}
	}

	if (m_bNoRootMove)
	{
		m_pOwnerTransform->Go_Straight(fTimeDelta, m_pNavigation);

		if (XMVector3Length(XMLoadFloat4(&m_vStartPos) - m_pOwnerTransform->Get_Position()).m128_f32[0] >= 4.f)
			m_bNoRootMove = false;
	}
	else
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CPlayerState_Execution_Default::Late_Update(_float fTimeDelta)
{

	if (m_pModel->Is_AnimComplete())
	{
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
		return;
	}
		
	PlayerState ePlayerState = Decide_State();
	Check_ExtraStateChange(ePlayerState);
}


void CPlayerState_Execution_Default::OnState_End()
{
	m_iExecutionCount = 0;
}

void CPlayerState_Execution_Default::Check_ExtraStateChange(PlayerState eState)
{
	if (PlayerState::State_End == eState)
		return;

	if (PlayerState::State_Execution_Default == eState)
	{
		CEnemy* pExecutionEnemy = m_pPlayer->Get_ExecutionEnemy();
		if (nullptr != pExecutionEnemy)
			OnState_Start(pExecutionEnemy);
		else
			m_pPlayer->Change_State((_uint)PlayerState::State_Attack);
	}
	else
	{
		m_pPlayer->Change_State((_uint)eState);
	}
}

CPlayerState_Execution_Default* CPlayerState_Execution_Default::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Execution_Default* pInstance = new CPlayerState_Execution_Default(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Execution_Default"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Execution_Default::Free()
{
	__super::Free();
}
