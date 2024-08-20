#include "UrdState_Step_F.h"

#include "Effect_Manager.h"

CUrdState_Step_F::CUrdState_Step_F(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_Step_F::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUrdState_Step_F::OnState_Start(void* pArg)
{
	m_pOwnerTransform->LookAt2D(m_pTargetTransform->Get_Position());
	m_pUrd->Set_LookTarget(false);
	m_pUrd->Set_Stanced(true);
	m_pUrd->Set_Invincible(true);
	m_pUrd->Add_StepCount();

	if (false == m_pUrd->Is_PrevState_Step())
		m_eDir = DIR_END;
	else
		m_eDir = *(URDSTEP_DIR*)pArg;

	Decide_StepAnim();

	EFFECTMGR->Active_Effect("Effect_Urd_Step_Particle", &m_pUrd->Get_EffectSpawnDesc());

	m_pGameInstance->Play_RandomSound(L"Urd_MagicMove", 1, 2, false, 1.f);
}

void CUrdState_Step_F::Update(_float fTimeDelta)
{
	if (false == m_pUrd->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CUrdState_Step_F::Late_Update(_float fTimeDelta)
{

}


void CUrdState_Step_F::OnState_End()
{
	m_pUrd->Set_Invincible(false);
	if (false == m_pUrd->Can_StepState())
		m_pUrd->Reset_StepCount();
}

void CUrdState_Step_F::Decide_State()
{
	if (false == m_pUrd->Can_StepState())
		ADD_EVENT(bind(&CUrdState_Base::Decide_Attack, this));
	else
	{
		_int iRandNum = JoRandom::Random_Int(0, 2);

		if (iRandNum)
			ADD_EVENT(bind(&CUrdState_Base::Decide_Step, this));
		else
			ADD_EVENT(bind(&CUrdState_Base::Decide_Attack, this));
	}
}

void CUrdState_Step_F::Decide_StepAnim()
{
	if (NONE == m_eDir)
	{
		_int iRandNum = JoRandom::Random_Int(0, 1);
		if (iRandNum)
		{
			m_eDir = LEFT;
			m_pModel->Change_Animation(Urd_StepFL);
		}
		else
		{
			m_eDir = RIGHT;
			m_pModel->Change_Animation(Urd_StepFR);
		}
	}
	else if (LEFT == m_eDir)
	{
		m_eDir = RIGHT;
		m_pModel->Change_Animation(Urd_StepFR);
	}
	else
	{
		m_eDir = LEFT;
		m_pModel->Change_Animation(Urd_StepFL);
	}
}


CUrdState_Step_F* CUrdState_Step_F::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_Step_F* pInstance = new CUrdState_Step_F(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_Step_F"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_Step_F::Free()
{
	__super::Free();
}
