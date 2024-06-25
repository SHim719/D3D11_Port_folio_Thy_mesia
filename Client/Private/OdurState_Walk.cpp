#include "OdurState_Walk.h"

COdurState_Walk::COdurState_Walk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_Walk::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void COdurState_Walk::OnState_Start(void* pArg)
{
	m_pOdur->Set_LookTarget(true);
	m_pOdur->Set_Stanced(false);

	Decide_Animation();

	m_fPatternTime = pArg == nullptr ? JoRandom::Random_Float(1.5f, 2.5f): *(_float*)pArg;

	m_pOwnerTransform->Set_Speed(m_fWalkSpeed);
}

void COdurState_Walk::OnGoing(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_Animation();

	switch (m_iDir)
	{
	case 0:
		m_pOwnerTransform->Go_Straight(fTimeDelta, m_pNavigation);
		break;
	case 1:
		m_pOwnerTransform->Go_Left(fTimeDelta, m_pNavigation);
		break;
	case 2:
		m_pOwnerTransform->Go_Right(fTimeDelta, m_pNavigation);
		break;
	}
	
	m_fPatternTime -= fTimeDelta;
	if (m_fPatternTime <= 0.f)
		Decide_State();
}

void COdurState_Walk::OnState_End()
{

}

void COdurState_Walk::OnHit(void* pArg)
{
	m_pOdur->Change_State((_uint)OdurState::State_Hit);
}

void COdurState_Walk::Decide_Animation()
{
	m_iDir = 0;

	_float fDist = XMVector3Length(m_pTargetTransform->Get_Position() - m_pOwnerTransform->Get_Position()).m128_f32[0];
	if (fDist <= 4.f)
		m_iDir = JoRandom::Random_Int(1, 2);

	m_pModel->Change_Animation(Magician_Walk2F + m_iDir);
}

COdurState_Walk* COdurState_Walk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_Walk* pInstance = new COdurState_Walk(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_Walk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_Walk::Free()
{
	__super::Free();
}
