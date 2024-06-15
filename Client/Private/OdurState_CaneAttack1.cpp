#include "OdurState_CaneAttack1.h"

COdurState_CaneAttack1::COdurState_CaneAttack1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_CaneAttack1::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void COdurState_CaneAttack1::OnState_Start(void* pArg)
{
	m_pModel->Change_Animation(Magician_CaneAttack03);
}

void COdurState_CaneAttack1::OnGoing(_float fTimeDelta)
{
	//m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos());
}

void COdurState_CaneAttack1::OnState_End()
{
}


COdurState_CaneAttack1* COdurState_CaneAttack1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_CaneAttack1* pInstance = new COdurState_CaneAttack1(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_CaneAttack1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_CaneAttack1::Free()
{
}
