#include "OdurState_Idle.h"

COdurState_Idle::COdurState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_Idle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void COdurState_Idle::OnState_Start(void* pArg)
{
	m_pModel->Change_Animation(Magician_Idle);
}

void COdurState_Idle::OnGoing(_float fTimeDelta)
{
}

void COdurState_Idle::OnState_End()
{

}

void COdurState_Idle::OnHit(void* pArg)
{
	m_pOdur->Change_State((_uint)OdurState::State_Hit);
}

COdurState_Idle* COdurState_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_Idle* pInstance = new COdurState_Idle(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_Idle::Free()
{
}
