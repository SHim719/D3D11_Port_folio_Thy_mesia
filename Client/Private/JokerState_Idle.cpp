#include "JokerState_Idle.h"

CJokerState_Idle::CJokerState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CJokerState_Base(pDevice, pContext)
{
}

HRESULT CJokerState_Idle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CJokerState_Idle::OnState_Start(void* pArg)
{
	m_pModel->Change_Animation(Joker_Idle);
}

void CJokerState_Idle::Update(_float fTimeDelta)
{

}

void CJokerState_Idle::OnState_End()
{

}

CJokerState_Idle* CJokerState_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CJokerState_Idle* pInstance = new CJokerState_Idle(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CJokerState_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJokerState_Idle::Free()
{
	__super::Free();
}
