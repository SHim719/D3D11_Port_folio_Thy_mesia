#include "OdurState_Execute.h"

#include "Player.h"

COdurState_Execute::COdurState_Execute(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_Execute::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void COdurState_Execute::OnState_Start(void* pArg)
{
	m_pOdur->Set_LookTarget(false);
	m_pOdur->Set_Stanced(true);

	m_pModel->Change_Animation(Magician_SPAttack1_Suc, 0.f);
}

void COdurState_Execute::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void COdurState_Execute::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_State();
}

void COdurState_Execute::OnState_End()
{

}

COdurState_Execute* COdurState_Execute::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_Execute* pInstance = new COdurState_Execute(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_Execute"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_Execute::Free()
{
	__super::Free();
}
