#include "TwinBladeKnightState_Dodge.h"

CTwinBladeKnightState_Dodge::CTwinBladeKnightState_Dodge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTwinBladeKnightState_Base(pDevice, pContext)
{
}

HRESULT CTwinBladeKnightState_Dodge::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTwinBladeKnightState_Dodge::OnState_Start(void* pArg)
{
	m_pTwinBladeKnight->Set_LookTarget(false);
	m_pTwinBladeKnight->Set_Stanced(true);
	m_pOwnerTransform->LookAt2D(m_pTwinBladeKnight->Get_Target()->Get_Transform()->Get_Position());
	
	m_pModel->Change_Animation(LArmor_TwinSwords_Dodge_B);
}

void CTwinBladeKnightState_Dodge::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CTwinBladeKnightState_Dodge::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_State();
}

void CTwinBladeKnightState_Dodge::OnState_End()
{

}


CTwinBladeKnightState_Dodge* CTwinBladeKnightState_Dodge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTwinBladeKnightState_Dodge* pInstance = new CTwinBladeKnightState_Dodge(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTwinBladeKnightState_Dodge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTwinBladeKnightState_Dodge::Free()
{
	__super::Free();
}
