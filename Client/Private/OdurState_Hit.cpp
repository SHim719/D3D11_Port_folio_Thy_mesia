#include "OdurState_Hit.h"

COdurState_Hit::COdurState_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_Hit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void COdurState_Hit::OnState_Start(void* pArg)
{
	m_pModel->Change_Animation(Magician_HurtFL + m_iHitCount % 2);
}

void COdurState_Hit::OnGoing(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		Decide_State();
		return;
	}
		
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos());
}

void COdurState_Hit::OnState_End()
{
}

void COdurState_Hit::OnHit(void* pArg)
{
	++m_iHitCount;
	OnState_Start(nullptr);
}

COdurState_Hit* COdurState_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_Hit* pInstance = new COdurState_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_Hit::Free()
{
	__super::Free();
}
