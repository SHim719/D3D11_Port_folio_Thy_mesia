#include "TwinBladeKnightState_Walk.h"

CTwinBladeKnightState_Walk::CTwinBladeKnightState_Walk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTwinBladeKnightState_Base(pDevice, pContext)
{
}

HRESULT CTwinBladeKnightState_Walk::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTwinBladeKnightState_Walk::OnState_Start(void* pArg)
{
	m_pTwinBladeKnight->Set_LookTarget(true);
	m_pTwinBladeKnight->Set_Stanced(false);

	m_fTimeAcc = 0.f;
	m_fWalkTime = JoRandom::Random_Float(1.f, 2.f);
	m_pOwnerTransform->Set_Speed(m_fWalkSpeed);

	Decide_WalkAnimation();
}

void CTwinBladeKnightState_Walk::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	Move(fTimeDelta);
}

void CTwinBladeKnightState_Walk::Late_Update(_float fTimeDelta)
{
	if (m_fTimeAcc >= m_fWalkTime)
	{
		m_fTimeAcc = 0.f;
		Decide_State();
	}
}

void CTwinBladeKnightState_Walk::OnState_End()
{

}

void CTwinBladeKnightState_Walk::Decide_WalkAnimation()
{
	_float fDist = XMVector3Length(m_pTargetTransform->Get_Position() - m_pOwnerTransform->Get_Position()).m128_f32[0];

	if (fDist >= 3.f)
	{
		if (LArmor_TwinSwords_WalkF == m_pModel->Get_CurrentAnimIndex())
			return;
		m_pModel->Change_Animation(LArmor_TwinSwords_WalkF, 0.f);
		m_iDir = 0;
	}
	else 
	{
		if (LArmor_TwinSwords_WalkB == m_pModel->Get_CurrentAnimIndex())
			return;

		m_pModel->Change_Animation(LArmor_TwinSwords_WalkB, 0.f);
		m_iDir = 1;
	}
}

void CTwinBladeKnightState_Walk::Move(_float fTimeDelta)
{
	switch (m_iDir)
	{
	case 0:
		if (false == m_pTwinBladeKnight->Is_CollPlayer())
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

CTwinBladeKnightState_Walk* CTwinBladeKnightState_Walk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTwinBladeKnightState_Walk* pInstance = new CTwinBladeKnightState_Walk(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTwinBladeKnightState_Walk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTwinBladeKnightState_Walk::Free()
{
	__super::Free();
}
