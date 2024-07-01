#include "OdurState_ExecutionDisappear.h"

COdurState_ExecutionDisappear::COdurState_ExecutionDisappear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_ExecutionDisappear::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void COdurState_ExecutionDisappear::OnState_Start(void* pArg)
{
	m_pOdur->Set_LookTarget(true);
	m_pOdur->Set_Stanced(true);

	m_pOdur->Set_Alpha(0.f);

	m_pModel->Change_Animation(Magician_DisappearMove_F);
}

void COdurState_ExecutionDisappear::Update(_float fTimeDelta)
{
	_float fDist = XMVectorGetX(XMVector3Length(m_pOdur->Get_Target()->Get_Transform()->Get_Position() - m_pOwnerTransform->Get_Position()));

	if (fDist < 3.5f)
		m_pOdur->Change_State((_uint)OdurState::State_ReadyExecution);

	if (false == m_pOdur->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos() * 2.f, m_pNavigation);
}

void COdurState_ExecutionDisappear::OnState_End()
{

}

COdurState_ExecutionDisappear* COdurState_ExecutionDisappear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_ExecutionDisappear* pInstance = new COdurState_ExecutionDisappear(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_ExecutionDisappear"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_ExecutionDisappear::Free()
{
	__super::Free();
}
