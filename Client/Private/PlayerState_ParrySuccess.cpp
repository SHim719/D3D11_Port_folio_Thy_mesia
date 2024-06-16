#include "PlayerState_ParrySuccess.h"

CPlayerState_ParrySuccess::CPlayerState_ParrySuccess(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_ParrySuccess::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Attack, PlayerState::State_Avoid, PlayerState::State_Parry };

    return S_OK;
}

void CPlayerState_ParrySuccess::OnState_Start(void* pArg)
{
	m_pPlayer->Disable_NextState();
	m_pPlayer->Disable_Rotation();
	m_pPlayer->Set_Invincible();

	_int* iParryDir = (_int*)pArg;
	_int iRandNum = JoRandom::Random_Int(0, 1);

	if (0 == *iParryDir)
		m_pModel->Change_Animation(Corvus_SD_ParryDeflect_L + iRandNum, 0.f);
	else
		m_pModel->Change_Animation(Corvus_SD_ParryDeflect_R + iRandNum, 0.f);

	
}

void CPlayerState_ParrySuccess::OnGoing(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);

}

void CPlayerState_ParrySuccess::OnState_End()
{
}

void CPlayerState_ParrySuccess::OnHit(void* pArg)
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
