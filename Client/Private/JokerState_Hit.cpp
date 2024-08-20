#include "JokerState_Hit.h"

CJokerState_Hit::CJokerState_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CJokerState_Base(pDevice, pContext)
{
}

HRESULT CJokerState_Hit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CJokerState_Hit::OnState_Start(void* pArg)
{
	m_pJoker->Set_LookTarget(true);


	m_pModel->Change_Animation(Joker_HurtCounter);
}

void CJokerState_Hit::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CJokerState_Hit::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_State();

}

void CJokerState_Hit::OnState_End()
{
	Setup_RootRotation();
}

CJokerState_Hit* CJokerState_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CJokerState_Hit* pInstance = new CJokerState_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CJokerState_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJokerState_Hit::Free()
{
	__super::Free();
}
