#include "PlayerState_Climb_Start.h"

CPlayerState_Climb_Start::CPlayerState_Climb_Start(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Climb_Start::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { };

	return S_OK;
}

void CPlayerState_Climb_Start::OnState_Start(void* pArg)
{
	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_Stanced(true);

	m_pPlayer->Set_Adjust_NaviY(false);
	m_pPlayer->Set_Active_DefaultWeapons(false);

	_uint iAnimIdx = *(_uint*)pArg;

	m_pModel->Change_Animation(iAnimIdx);
}

void CPlayerState_Climb_Start::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos());
}

void CPlayerState_Climb_Start::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		_int iDir = 0;
		m_pPlayer->Change_State((_uint)PlayerState::State_Climb_Idle, &iDir);
	}
		
}

void CPlayerState_Climb_Start::OnState_End()
{

}

CPlayerState_Climb_Start* CPlayerState_Climb_Start::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Climb_Start* pInstance = new CPlayerState_Climb_Start(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Climb_Start"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Climb_Start::Free()
{
	__super::Free();
}
