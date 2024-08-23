#include "UrdState_Stunned_Start.h"

#include "Player.h"

CUrdState_Stunned_Start::CUrdState_Stunned_Start(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_Stunned_Start::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUrdState_Stunned_Start::OnState_Start(void* pArg)
{
	m_pUrd->Set_LookTarget(false);
	m_pOwnerTransform->LookAt2D(m_pTargetTransform->Get_Position());

	RIMLIGHTDESC RimDesc{};
	RimDesc.bColorLerp = true;
	RimDesc.fDuration = 1.f;
	RimDesc.fRimPower = 1.f;
	RimDesc.fRimStrength = 3.f;
	RimDesc.vRimColor = { 0.f, 1.f, 0.6f, 1.f };

	m_pUrd->Active_RimLight(RimDesc);
	m_pUrd->Set_NowWeapon_Disappear();
	m_pUrd->Set_Adjust_NaviY(true);
	m_pUrd->Set_Active_AllWeapon_Colliders(false);

	m_pGameInstance->Set_TimeScaleWithRealTime(0.4f, 1.f);

	EFFECTMGR->Active_Effect("Effect_Enemy_Parry_Particle", &m_pUrd->Get_EffectSpawnDesc());

	PLAY_SOUND(L"Stunned_Start", false, 1.f);

	m_pModel->Change_Animation(Urd_StunStart);
}

void CUrdState_Stunned_Start::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CUrdState_Stunned_Start::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pUrd->Change_State((_uint)UrdState::State_Stunned_Loop);

}

void CUrdState_Stunned_Start::OnState_End()
{
}

void CUrdState_Stunned_Start::OnHit(const ATTACKDESC& AttackDesc)
{
}

CUrdState_Stunned_Start* CUrdState_Stunned_Start::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_Stunned_Start* pInstance = new CUrdState_Stunned_Start(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_Stunned_Start"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_Stunned_Start::Free()
{
	__super::Free();
}
