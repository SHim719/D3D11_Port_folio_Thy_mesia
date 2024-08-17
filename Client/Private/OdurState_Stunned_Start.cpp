#include "OdurState_Stunned_Start.h"

#include "Player.h"

COdurState_Stunned_Start::COdurState_Stunned_Start(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_Stunned_Start::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void COdurState_Stunned_Start::OnState_Start(void* pArg)
{
	m_pOdur->Set_LookTarget(false);
	m_pOdur->Set_Alpha(1.f);

	RIMLIGHTDESC RimDesc{};
	RimDesc.bColorLerp = true;
	RimDesc.fDuration = 1.f;
	RimDesc.fRimPower = 1.f;
	RimDesc.fRimStrength = 3.f;
	RimDesc.vRimColor = { 0.f, 1.f, 0.6f, 1.f };

	m_pOdur->Active_RimLight(RimDesc);

	m_pGameInstance->Set_TimeScaleWithRealTime(0.4f, 1.f);

	m_pModel->Change_Animation(Magician_StunStart_Cane);
}

void COdurState_Stunned_Start::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void COdurState_Stunned_Start::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pOdur->Change_State((_uint)OdurState::State_Stunned_Loop);

}

void COdurState_Stunned_Start::OnState_End()
{
}

void COdurState_Stunned_Start::OnHit(const ATTACKDESC& AttackDesc)
{
}

COdurState_Stunned_Start* COdurState_Stunned_Start::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_Stunned_Start* pInstance = new COdurState_Stunned_Start(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_Stunned_Start"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_Stunned_Start::Free()
{
	__super::Free();
}
