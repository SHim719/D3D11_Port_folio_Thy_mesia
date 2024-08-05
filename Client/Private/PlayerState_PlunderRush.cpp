#include "PlayerState_PlunderRush.h"

CPlayerState_PlunderRush::CPlayerState_PlunderRush(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_PlunderRush::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { PlayerState::State_Avoid, PlayerState::State_Parry };

	return S_OK;
}

void CPlayerState_PlunderRush::OnState_Start(void* pArg)
{
	Reset_AttackIdx();

	m_pPlayer->Set_CanNextState(false);
	m_pPlayer->Set_CanRotation(true);
	m_pPlayer->Set_Active_Claws(true);
	m_pPlayer->Set_Active_DefaultWeapons(false);

	m_pPlayer->Update_AttackDesc();

	RADIALBLUR_DESCS RadialDescs{};
	RadialDescs.fBlurRadius = 10.f;
	RadialDescs.fBlurStrength = 1.5f;

	m_pGameInstance->Active_RadialBlur(RadialDescs);
	m_pGameInstance->Update_BlurCenterWorld(m_pPlayer->Get_Center());

	m_pModel->Change_AnimationWithStartFrame(Corvus_Raven_ClawLong_ChargeFull, 40, 0.05f);
}

void CPlayerState_PlunderRush::Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);


	if (m_pPlayer->Can_Rotation() && false == m_pPlayer->Is_LockOn())
	{
		_vector vNewLook = Calc_MoveLook(true);

		if (0.f != vNewLook.m128_f32[0] || 0.f != vNewLook.m128_f32[1])
			Rotate_To_Look(vNewLook, fTimeDelta);
	}


	if (!m_pPlayer->Is_CollEnemy())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);

}

void CPlayerState_PlunderRush::Late_Update(_float fTimeDelta)
{
	PlayerState ePlayerState = Decide_State();
	if (PlayerState::State_End != ePlayerState)
		Check_ExtraStateChange(ePlayerState);
}


void CPlayerState_PlunderRush::OnState_End()
{
	m_pPlayer->Set_Active_DefaultWeapons(true);
	m_pPlayer->Set_Active_Claws(false);

	m_pGameInstance->Inactive_RadialBlur(0.5f);
}

void CPlayerState_PlunderRush::Init_AttackDesc()
{
	m_AttackDescs.reserve(1);

	ATTACKDESC AtkDesc{};
	AtkDesc.ePlayerAttackType = PLUNDER;
	
	m_AttackDescs.emplace_back(CPlayer::CLAW_R, AtkDesc);
}


CPlayerState_PlunderRush* CPlayerState_PlunderRush::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_PlunderRush* pInstance = new CPlayerState_PlunderRush(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_PlunderRush"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_PlunderRush::Free()
{
	__super::Free();
}
