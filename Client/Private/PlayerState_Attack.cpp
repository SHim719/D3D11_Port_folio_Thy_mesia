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

	return S_OK;
}

void CPlayerState_Attack::OnState_Start()
{
	//_float fBlendingTime = m_iNowComboCnt == 0 ? 0.02f : 0.f;
	m_pModel->Change_Animation((_uint)(Corvus_SD_LAttack1 + m_iNowComboCnt), 0.f);
	m_bCanNextAttack = false;
}

void CPlayerState_Attack::OnGoing(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		m_pPlayer->Change_State(PlayerState::State_Idle);
		return;
	}
		
	if (m_bCanNextAttack && m_iNowComboCnt < m_iMaxComboCnt)
	{
		if (KEY_PUSHING(eKeyCode::LButton))
		{
			m_iNowComboCnt++;
			OnState_Start();
		}
			
	}


	m_pOwnerTransform->Go_Root(m_pModel->Get_DeltaRootPos(), m_pOwnerTransform->Get_MoveLook());
}

void CPlayerState_Attack::OnState_End()
{
	m_iNowComboCnt = 0;
}

void CPlayerState_Attack::Enable_NextAttack()
{
	m_bCanNextAttack = true;
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
