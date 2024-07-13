#include "TwinBladeKnightState_Stunned_Loop.h"

#include "Player.h"

CTwinBladeKnightState_Stunned_Loop::CTwinBladeKnightState_Stunned_Loop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTwinBladeKnightState_Base(pDevice, pContext)
{
}

HRESULT CTwinBladeKnightState_Stunned_Loop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTwinBladeKnightState_Stunned_Loop::OnState_Start(void* pArg)
{
	static_cast<CPlayer*>(m_pTwinBladeKnight->Get_Target())->Add_StunnedEnemy(m_pTwinBladeKnight);

	m_pModel->Change_Animation(LArmor_TwinSwords_HurtStunLoop);
}

void CTwinBladeKnightState_Stunned_Loop::Update(_float fTimeDelta)
{

}

void CTwinBladeKnightState_Stunned_Loop::Late_Update(_float fTimeDelta)
{
}

void CTwinBladeKnightState_Stunned_Loop::OnState_End()
{
}

void CTwinBladeKnightState_Stunned_Loop::OnHit(const ATTACKDESC& AttackDesc)
{

}

CTwinBladeKnightState_Stunned_Loop* CTwinBladeKnightState_Stunned_Loop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTwinBladeKnightState_Stunned_Loop* pInstance = new CTwinBladeKnightState_Stunned_Loop(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTwinBladeKnightState_Stunned_Loop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTwinBladeKnightState_Stunned_Loop::Free()
{
	__super::Free();
}
