#include "OdurState_DisappearMove.h"

COdurState_DisappearMove::COdurState_DisappearMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_DisappearMove::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModel->Bind_Func("Odur_ChangeToAppear", bind(&COdurState_DisappearMove::ChangeState_Appear, this));

	return S_OK;
}

void COdurState_DisappearMove::OnState_Start(void* pArg)
{
	m_pOdur->Set_LookTarget(false);
	m_pOdur->Set_Stanced(true);

  	m_pOdur->Set_Alpha(0.f);

	if (nullptr != pArg)
	{
		m_iDir = 1;
		m_pModel->Change_Animation(Magician_DisappearMove_B);
	}
	else
	{
		Decide_Animation();
	}

	EFFECTMGR->Active_Effect("Effect_Odur_Disappear_Particle", &m_pOdur->Get_EffectSpawnDesc());

	PLAY_SOUND(L"Odur_DisappearMove", false, 1.f);
}

void COdurState_DisappearMove::Update(_float fTimeDelta)
{
	if (0 == m_iDir && m_pOdur->Is_CollPlayer())
		return;

	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void COdurState_DisappearMove::OnState_End()
{

}

void COdurState_DisappearMove::Decide_Animation()
{
	// 카드던지는 패턴 구분해서 만들기
	m_iDir = 0;

	_float fDist = XMVector3Length(m_pTargetTransform->Get_Position() - m_pOwnerTransform->Get_Position()).m128_f32[0];
	if (fDist >= 6.f)
		m_pModel->Change_Animation(Magician_DisappearMove_F);
	else
	{
		m_iDir = JoRandom::Random_Int(1, 3);

		switch (m_iDir)
		{
		case 1:
			m_pModel->Change_Animation(Magician_DisappearMove_B);
			break;
		case 2:
			m_pModel->Change_Animation(Magician_DisappearMove_L);
			break;
		case 3:
			m_pModel->Change_Animation(Magician_DisappearMove_R);
			break;
		}
	}
	
	
}

void COdurState_DisappearMove::ChangeState_Appear()
{
	ADD_EVENT(bind(&COdur::Change_State, m_pOdur, (_uint)OdurState::State_Appear, &m_iDir));
}


COdurState_DisappearMove* COdurState_DisappearMove::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_DisappearMove* pInstance = new COdurState_DisappearMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_DisappearMove"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_DisappearMove::Free()
{
	__super::Free();
}
