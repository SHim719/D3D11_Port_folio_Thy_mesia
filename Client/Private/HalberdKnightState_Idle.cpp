#include "HalberdKnightState_Idle.h"

CHalberdKnightState_Idle::CHalberdKnightState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CHalberdKnightState_Base(pDevice, pContext)
{
}

HRESULT CHalberdKnightState_Idle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CHalberdKnightState_Idle::OnState_Start(void* pArg)
{
	m_pModel->Change_Animation(HArmorLV1_Halberds_Idle);
}

void CHalberdKnightState_Idle::Update(_float fTimeDelta)
{

}

void CHalberdKnightState_Idle::OnState_End()
{

}

CHalberdKnightState_Idle* CHalberdKnightState_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CHalberdKnightState_Idle* pInstance = new CHalberdKnightState_Idle(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHalberdKnightState_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHalberdKnightState_Idle::Free()
{
	__super::Free();
}
