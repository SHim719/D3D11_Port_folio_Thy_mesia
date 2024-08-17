#include "HalberdKnightState_Stunned_Loop.h"

#include "Player.h"

CHalberdKnightState_Stunned_Loop::CHalberdKnightState_Stunned_Loop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CHalberdKnightState_Base(pDevice, pContext)
{
}

HRESULT CHalberdKnightState_Stunned_Loop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CHalberdKnightState_Stunned_Loop::OnState_Start(void* pArg)
{
	m_pModel->Change_Animation(HArmorLV1_Halberds_HurtStunLoop);
}

void CHalberdKnightState_Stunned_Loop::Update(_float fTimeDelta)
{

}

void CHalberdKnightState_Stunned_Loop::Late_Update(_float fTimeDelta)
{
}

void CHalberdKnightState_Stunned_Loop::OnState_End()
{
}

void CHalberdKnightState_Stunned_Loop::OnHit(const ATTACKDESC& AttackDesc)
{

}

CHalberdKnightState_Stunned_Loop* CHalberdKnightState_Stunned_Loop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CHalberdKnightState_Stunned_Loop* pInstance = new CHalberdKnightState_Stunned_Loop(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHalberdKnightState_Stunned_Loop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHalberdKnightState_Stunned_Loop::Free()
{
	__super::Free();
}
