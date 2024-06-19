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
	m_pOdur->Set_Alpha_Increase();

	Decide_Animation(pArg);
}

void COdurState_Appear::OnGoing(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		Decide_State();
		return;
	}

	if (false == m_pOdur->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos());
}

void COdurState_Appear::OnState_End()
{

}

void COdurState_Appear::Decide_State()
{
	if (m_pOdur->Is_CardPattern())
	{
		m_pOdur->Set_CardPattern(false);
		m_pOdur->Change_State((_uint)OdurState::State_ThrowCard);
	}
	else
	{
		Decide_Attack();
	}
}

void COdurState_Appear::Decide_Animation(void* pArg)
{
	_int* iDir = (_int*)pArg;

	switch (*iDir)
	{
	case 0:
		m_pModel->Change_Animation(Magician_Appear_F, 0.f);
		break;
	case 1:
		m_pModel->Change_Animation(Magician_Appear_B, 0.f);
		break;
	case 2:
		m_pModel->Change_Animation(Magician_Appear_L, 0.f);
		break;
	case 3:
		m_pModel->Change_Animation(Magician_Appear_R, 0.f);
		break;
	}
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
