#include "TwinBladeKnightState_Stunned_Start.h"

#include "Player.h"

CTwinBladeKnightState_Stunned_Start::CTwinBladeKnightState_Stunned_Start(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTwinBladeKnightState_Base(pDevice, pContext)
{
}

HRESULT CTwinBladeKnightState_Stunned_Start::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTwinBladeKnightState_Stunned_Start::OnState_Start(void* pArg)
{
	m_pTwinBladeKnight->Set_LookTarget(false);

	RIMLIGHTDESC RimDesc{};
	RimDesc.bColorLerp = true;
	RimDesc.fDuration = 2.f;
	RimDesc.fRimPower = 1.f;
	RimDesc.fRimStrength = 3.f;
	RimDesc.vRimColor = { 0.f, 1.f, 0.6f, 1.f };

	m_pTwinBladeKnight->Active_RimLight(RimDesc);

	m_pGameInstance->Set_TimeScaleWithRealTime(0.2f, 2.f);

	m_pModel->Change_Animation(LArmor_TwinSwords_HurtStunStart);
}

void CTwinBladeKnightState_Stunned_Start::Update(_float fTimeDelta)
{

	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CTwinBladeKnightState_Stunned_Start::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pTwinBladeKnight->Change_State((_uint)TwinBladeKnight_State::State_Stunned_Loop);

}

void CTwinBladeKnightState_Stunned_Start::OnState_End()
{
}

void CTwinBladeKnightState_Stunned_Start::OnHit(const ATTACKDESC& AttackDesc)
{
}

CTwinBladeKnightState_Stunned_Start* CTwinBladeKnightState_Stunned_Start::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTwinBladeKnightState_Stunned_Start* pInstance = new CTwinBladeKnightState_Stunned_Start(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTwinBladeKnightState_Stunned_Start"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTwinBladeKnightState_Stunned_Start::Free()
{
	__super::Free();
}
