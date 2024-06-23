#include "OdurState_DisappearWalk.h"

COdurState_DisappearWalk::COdurState_DisappearWalk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_DisappearWalk::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void COdurState_DisappearWalk::OnState_Start(void* pArg)
{
	m_pOdur->Enable_LookTarget();
	m_pOdur->Enable_Stanced();
	m_pOdur->Set_Alpha_Decrease();

	Decide_Animation();
}

void COdurState_DisappearWalk::OnGoing(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		m_pOdur->Change_State((_uint)OdurState::State_DisappearMove);
		return;
	}
		
	if (false == m_pOdur->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void COdurState_DisappearWalk::OnState_End()
{

}

void COdurState_DisappearWalk::Decide_Animation()
{	
	_float fDist = XMVector3Length(m_pTargetTransform->Get_Position() - m_pOwnerTransform->Get_Position()).m128_f32[0];
	if (fDist >= 6.f)
		m_iDir = 1;
	else if (fDist <= 2.f)
		m_iDir = 0;
	else
		m_iDir = JoRandom::Random_Int(2, 3);

	m_pModel->Change_Animation(Magician_WalkBDisappear + m_iDir);
}

COdurState_DisappearWalk* COdurState_DisappearWalk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_DisappearWalk* pInstance = new COdurState_DisappearWalk(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_DisappearWalk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_DisappearWalk::Free()
{
	__super::Free();
}
