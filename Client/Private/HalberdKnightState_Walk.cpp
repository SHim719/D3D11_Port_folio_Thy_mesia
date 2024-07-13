#include "HalberdKnightState_Walk.h"

CHalberdKnightState_Walk::CHalberdKnightState_Walk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CHalberdKnightState_Base(pDevice, pContext)
{
}

HRESULT CHalberdKnightState_Walk::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CHalberdKnightState_Walk::OnState_Start(void* pArg)
{
	m_pHalberdKnight->Set_LookTarget(true);
	m_pHalberdKnight->Set_Stanced(false);

	m_fTimeAcc = 0.f;
	m_fWalkTime = JoRandom::Random_Float(1.f, 2.f);
	m_pOwnerTransform->Set_Speed(m_fWalkSpeed);

	Decide_WalkAnimation();

}

void CHalberdKnightState_Walk::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	Move(fTimeDelta);
}

void CHalberdKnightState_Walk::Late_Update(_float fTimeDelta)
{
	Decide_State();
}

void CHalberdKnightState_Walk::OnState_End()
{

}

void CHalberdKnightState_Walk::Decide_WalkAnimation()
{
	_float fDist = XMVector3Length(m_pTargetTransform->Get_Position() - m_pOwnerTransform->Get_Position()).m128_f32[0];
	if (fDist >= 7.f)
	{
		m_pModel->Change_Animation(HArmorLV1_Halberds_WalkF);
		m_iDir = 0;
	}
	
	else if (fDist < 1.f)
	{
		m_pModel->Change_Animation(HArmorLV1_Halberds_WalkB);
		m_iDir = 1;
	}
	else
	{
		m_iDir = JoRandom::Random_Int(2, 3);
	
		m_pModel->Change_Animation(HArmorLV1_Halberds_WalkB + m_iDir);
	}
}

void CHalberdKnightState_Walk::Decide_State()
{
	_float fDist = XMVector3Length(m_pTargetTransform->Get_Position() - m_pOwnerTransform->Get_Position()).m128_f32[0];

	//if (fDist > 5.f) ¾ê´Â °Å¸®ÀÓ
	//	m_pHalberdKnight->Change_State((_uint))

	if (m_fTimeAcc >= m_fWalkTime)
	{
		m_fTimeAcc = 0.f;
		_int iRandNum = JoRandom::Random_Int(0, 1);

		if (iRandNum)
			OnState_Start(nullptr);
		else
			Decide_Attack();
	}
}

void CHalberdKnightState_Walk::Move(_float fTimeDelta)
{
	switch (m_iDir)
	{
	case 0:
		if (false == m_pHalberdKnight->Is_CollPlayer())
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

CHalberdKnightState_Walk* CHalberdKnightState_Walk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CHalberdKnightState_Walk* pInstance = new CHalberdKnightState_Walk(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHalberdKnightState_Walk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHalberdKnightState_Walk::Free()
{
	__super::Free();
}
