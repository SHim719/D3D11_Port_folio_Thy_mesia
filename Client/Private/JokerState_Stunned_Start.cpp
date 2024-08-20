#include "JokerState_Stunned_Start.h"

CJokerState_Stunned_Start::CJokerState_Stunned_Start(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CJokerState_Base(pDevice, pContext)
{
}

HRESULT CJokerState_Stunned_Start::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CJokerState_Stunned_Start::OnState_Start(void* pArg)
{
	m_pJoker->Set_LookTarget(false);
	m_pOwnerTransform->LookAt2D(m_pTargetTransform->Get_Position());

	RIMLIGHTDESC RimDesc{};
	RimDesc.bColorLerp = true;
	RimDesc.fDuration = 0.5f;
	RimDesc.fRimPower = 1.f;
	RimDesc.fRimStrength = 3.f;
	RimDesc.vRimColor = { 0.f, 1.f, 0.6f, 1.f };

	m_pJoker->Active_RimLight(RimDesc);

	m_pGameInstance->Set_TimeScaleWithRealTime(0.2f, 1.f);

	PLAY_SOUND(L"Stunned_Start", false, 1.f);
	m_pModel->Change_Animation(Joker_Stun_Start);
}

void CJokerState_Stunned_Start::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CJokerState_Stunned_Start::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pJoker->Change_State((_uint)JokerState::State_Stunned_Loop);

}

void CJokerState_Stunned_Start::OnState_End()
{
}

void CJokerState_Stunned_Start::OnHit(const ATTACKDESC& AttackDesc)
{

}

CJokerState_Stunned_Start* CJokerState_Stunned_Start::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CJokerState_Stunned_Start* pInstance = new CJokerState_Stunned_Start(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CJokerState_Stunned_Start"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJokerState_Stunned_Start::Free()
{
	__super::Free();
}
