#include "JokerState_Executed_End.h"
#include "Bone.h"


CJokerState_Executed_End::CJokerState_Executed_End(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CJokerState_Base(pDevice, pContext)
{
}

HRESULT CJokerState_Executed_End::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CJokerState_Executed_End::OnState_Start(void* pArg)
{
	m_pModel->Change_Animation(Joker_TakeExecution_Loop, 0.f, true);
}

void CJokerState_Executed_End::Update(_float fTimeDelta)
{

}

void CJokerState_Executed_End::Late_Update(_float fTimeDelta)
{

}

void CJokerState_Executed_End::OnState_End()
{

}


CJokerState_Executed_End* CJokerState_Executed_End::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CJokerState_Executed_End* pInstance = new CJokerState_Executed_End(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CJokerState_Executed_End"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJokerState_Executed_End::Free()
{
	__super::Free();
}
