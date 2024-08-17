#include "TwinBladeKnightState_Idle.h"

CTwinBladeKnightState_Idle::CTwinBladeKnightState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTwinBladeKnightState_Base(pDevice, pContext)
{
}

HRESULT CTwinBladeKnightState_Idle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTwinBladeKnightState_Idle::OnState_Start(void* pArg)
{
	m_pModel->Change_Animation(LArmor_TwinSwords_IdleN);
	m_pTwinBladeKnight->Set_Stanced(true);
}

void CTwinBladeKnightState_Idle::Update(_float fTimeDelta)
{

}

void CTwinBladeKnightState_Idle::Late_Update(_float fTimeDelta)
{
	if (LArmor_TwinSwords_IdleN2Fight == m_pModel->Get_CurrentAnimIndex() && m_pModel->Is_AnimComplete())
		Decide_Attack();
}

void CTwinBladeKnightState_Idle::OnState_End()
{
	m_pTwinBladeKnight->Set_Stanced(false);
}

CTwinBladeKnightState_Idle* CTwinBladeKnightState_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTwinBladeKnightState_Idle* pInstance = new CTwinBladeKnightState_Idle(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTwinBladeKnightState_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTwinBladeKnightState_Idle::Free()
{
	__super::Free();
}
