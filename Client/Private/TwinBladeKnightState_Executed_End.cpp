#include "TwinBladeKnightState_Executed_End.h"
#include "Bone.h"


CTwinBladeKnightState_Executed_End::CTwinBladeKnightState_Executed_End(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTwinBladeKnightState_Base(pDevice, pContext)
{
}

HRESULT CTwinBladeKnightState_Executed_End::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTwinBladeKnightState_Executed_End::OnState_Start(void* pArg)
{
	m_pTwinBladeKnight->Active_Dissolve();
	PLAY_SOUND(L"TwinBlade_Die", false, 1.f);
}

void CTwinBladeKnightState_Executed_End::Update(_float fTimeDelta)
{
}

void CTwinBladeKnightState_Executed_End::Late_Update(_float fTimeDelta)
{
}

void CTwinBladeKnightState_Executed_End::OnState_End()
{

}


CTwinBladeKnightState_Executed_End* CTwinBladeKnightState_Executed_End::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTwinBladeKnightState_Executed_End* pInstance = new CTwinBladeKnightState_Executed_End(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTwinBladeKnightState_Executed_End"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTwinBladeKnightState_Executed_End::Free()
{
	__super::Free();
}
