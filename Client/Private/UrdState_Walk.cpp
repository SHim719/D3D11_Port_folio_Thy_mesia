#include "UrdState_Walk.h"

CUrdState_Walk::CUrdState_Walk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_Walk::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUrdState_Walk::OnState_Start(void* pArg)
{
	m_pUrd->Set_LookTarget(true);
	m_pUrd->Set_Stanced(false);

	Decide_Animation();

	m_fPatternTime = pArg == nullptr ? JoRandom::Random_Float(1.5f, 2.f) : *(_float*)pArg;

	m_pOwnerTransform->Set_Speed(m_fWalkSpeed);
}

void CUrdState_Walk::Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_Animation();

	switch (m_iDir)
	{
	case 0:
		if (false == m_pUrd->Is_CollPlayer())
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
	
}

void CUrdState_Walk::Late_Update(_float fTimeDelta)
{
	if (m_fPatternTime <= 0.f)
		Decide_State();
}

void CUrdState_Walk::OnState_End()
{

}


void CUrdState_Walk::Decide_Animation()
{
	m_iDir = 0;

	_float fDist = XMVector3Length(m_pTargetTransform->Get_Position() - m_pOwnerTransform->Get_Position()).m128_f32[0];
	if (fDist <= 4.f)
		m_iDir = JoRandom::Random_Int(1, 2);

	m_pModel->Change_Animation(Urd_WalkF + m_iDir);
}

CUrdState_Walk* CUrdState_Walk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_Walk* pInstance = new CUrdState_Walk(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_Walk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_Walk::Free()
{
	__super::Free();
}
