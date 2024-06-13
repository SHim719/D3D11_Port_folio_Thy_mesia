#include "PlayerState_Attack.h"


CPlayerState_Attack::CPlayerState_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Attack::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModel->Bind_Func("Enable_NextAttack", bind(&CPlayerState_Attack::Enable_NextAttack, this));
	m_pModel->Bind_Func("Disable_NextAttack", bind(&CPlayerState_Attack::Disable_NextAttack, this));
	m_pModel->Bind_Func("Disable_AttackRotation", bind(&CPlayerState_Attack::Disable_Rotation, this));

	return S_OK;
}

void CPlayerState_Attack::OnState_Start()
{
	if (3 == m_iNowComboCnt && 5 == m_pPlayerStat->Get_MaxAttackCnt())
		++m_iNowComboCnt;

	
	m_pModel->Change_Animation(_uint(Corvus_SD_LAttack1 + m_iNowComboCnt), 0.f);
	m_bCanNextAttack = false;
	m_bCanRotation = true;
}

void CPlayerState_Attack::OnGoing(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
		return;
	}

	if (m_bCanNextAttack && m_iNowComboCnt < m_pPlayerStat->Get_MaxAttackCnt())
	{
		if (KEY_PUSHING(eKeyCode::LButton))
		{
			m_iNowComboCnt++;
			OnState_Start();
		}
	}

	if (m_bCanRotation)
	{
		_vector vNewLook = Calc_MoveLook(true);

		if (0.f != vNewLook.m128_f32[0] || 0.f != vNewLook.m128_f32[1])
			Rotate_To_Look(vNewLook, fTimeDelta);
	}
	
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos());
}

void CPlayerState_Attack::OnState_End()
{
	m_iNowComboCnt = 0;
}

void CPlayerState_Attack::Enable_NextAttack()
{
	m_bCanNextAttack = true;
}

void CPlayerState_Attack::Disable_NextAttack()
{
	m_bCanNextAttack = false;
}


void CPlayerState_Attack::Disable_Rotation()
{
	m_bCanRotation = false;
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
