#include "JokerState_Walk.h"

CJokerState_Walk::CJokerState_Walk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CJokerState_Base(pDevice, pContext)
{
}

HRESULT CJokerState_Walk::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CJokerState_Walk::OnState_Start(void* pArg)
{
	m_pJoker->Set_LookTarget(true);

	m_fTimeAcc = 0.f;
	m_fWalkTime = JoRandom::Random_Float(1.f, 2.f);
	m_pOwnerTransform->Set_Speed(m_fWalkSpeed);

	Decide_WalkAnimation();
}

void CJokerState_Walk::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	Move(fTimeDelta);
}

void CJokerState_Walk::Late_Update(_float fTimeDelta)
{
	if (m_fTimeAcc >= m_fWalkTime)
	{
		m_fTimeAcc = 0.f;
		Decide_State();
	}
	
}

void CJokerState_Walk::OnState_End()
{

}

void CJokerState_Walk::Decide_WalkAnimation()
{
	_float fDist = XMVector3Length(m_pTargetTransform->Get_Position() - m_pOwnerTransform->Get_Position()).m128_f32[0];
	if (fDist >= 5.f)
	{
		m_pModel->Change_Animation(Joker_WalkF);
		m_iDir = 0;
	}
	
	else if (fDist < 2.2f)
	{
		m_pModel->Change_Animation(Joker_WalkB);
		m_iDir = 1;
	}
	else
	{
		m_iDir = JoRandom::Random_Int(2, 3);
	
		m_pModel->Change_Animation(Joker_WalkB + m_iDir);


	}
}


void CJokerState_Walk::Move(_float fTimeDelta)
{
	switch (m_iDir)
	{
	case 0:
		if (!m_pJoker->Is_CollPlayer())
			m_pOwnerTransform->Go_Straight(fTimeDelta, m_pNavigation);
		break;
	case 1:
		m_pOwnerTransform->Go_Backward(fTimeDelta, m_pNavigation);
		break;
	case 2:
		m_pOwnerTransform->Go_Left(fTimeDelta, m_pNavigation);
		break;
	case 3:
		m_pOwnerTransform->Go_Right(fTimeDelta, m_pNavigation);
		break;
	}

}

CJokerState_Walk* CJokerState_Walk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CJokerState_Walk* pInstance = new CJokerState_Walk(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CJokerState_Walk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJokerState_Walk::Free()
{
	__super::Free();
}
