#include "OdurState_Appear.h"

COdurState_Appear::COdurState_Appear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_Appear::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void COdurState_Appear::OnState_Start(void* pArg)
{
	m_pOdur->Enable_LookTarget();
	m_pOdur->Enable_Stanced();
	m_pOdur->Set_Alpha_Decrease();

	Decide_Animation();
}

void COdurState_Appear::OnGoing(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		m_pOdur->Change_State((_uint)OdurState::State_DisappearMove);
		return;
	}

	if (false == m_pOdur->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos());
}

void COdurState_Appear::OnState_End()
{

}

void COdurState_Appear::Decide_Animation()
{
	//_float fDist = XMVector3Length(m_pTargetTransform->Get_Position() - m_pOwnerTransform->Get_Position()).m128_f32[0];
	//if (fDist >= 6.f)
	//	m_iDir = 1;
	//else if (fDist <= 2.f)
	//	m_iDir = 0;
	//else
	//	m_iDir = JoRandom::Random_Int(2, 3);
	//
	//m_pModel->Change_Animation(Magician_WalkBDisappear + m_iDir);
}

COdurState_Appear* COdurState_Appear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_Appear* pInstance = new COdurState_Appear(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_Appear"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_Appear::Free()
{
	__super::Free();
}