#include "HalberdKnightState_Stunned_Start.h"

#include "Player.h"

CHalberdKnightState_Stunned_Start::CHalberdKnightState_Stunned_Start(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CHalberdKnightState_Base(pDevice, pContext)
{
}

HRESULT CHalberdKnightState_Stunned_Start::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CHalberdKnightState_Stunned_Start::OnState_Start(void* pArg)
{
	m_pOwnerTransform->LookAt2D(m_pTargetTransform->Get_Position());
	m_pHalberdKnight->Set_LookTarget(false);

	m_pModel->Change_Animation(HArmorLV1_Halberds_HurtStunStart);
}

void CHalberdKnightState_Stunned_Start::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CHalberdKnightState_Stunned_Start::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pHalberdKnight->Change_State((_uint)HalberdKnight_State::State_Stunned_Loop);

}

void CHalberdKnightState_Stunned_Start::OnState_End()
{
}

void CHalberdKnightState_Stunned_Start::OnHit(const ATTACKDESC& AttackDesc)
{
}


CHalberdKnightState_Stunned_Start* CHalberdKnightState_Stunned_Start::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CHalberdKnightState_Stunned_Start* pInstance = new CHalberdKnightState_Stunned_Start(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHalberdKnightState_Stunned_Start"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHalberdKnightState_Stunned_Start::Free()
{
	__super::Free();
}