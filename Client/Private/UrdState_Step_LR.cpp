#include "UrdState_Step_LR.h"

#include "Effect_Manager.h"

CUrdState_Step_LR::CUrdState_Step_LR(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_Step_LR::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUrdState_Step_LR::OnState_Start(void* pArg)
{
	m_pUrd->Set_LookTarget(true);
	m_pUrd->Set_Stanced(true);
	m_pUrd->Set_Invincible(true);
	m_pUrd->Add_StepCount();

	if (false == m_pUrd->Is_PrevState_Step())
		m_eDir = DIR_END;
	else
		m_eDir = *(URDSTEP_DIR*)pArg;

	Decide_StepAnim();

	EFFECTMGR->Active_Effect("Effect_Urd_Step_Particle", &m_pUrd->Get_EffectSpawnDesc());

	m_pGameInstance->Play_RandomSound(L"Urd_MagicMove", 1, 2, false, 0.8f);
}

void CUrdState_Step_LR::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CUrdState_Step_LR::Late_Update(_float fTimeDelta)
{

}


void CUrdState_Step_LR::OnState_End()
{
	m_pUrd->Set_Invincible(false);
	if (false == m_pUrd->Can_StepState())
		m_pUrd->Reset_StepCount();
}

void CUrdState_Step_LR::Decide_State()
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
void CUrdState_Step_LR::Decide_StepAnim()
{
	if (NONE == m_eDir)
	{
		_int iRandNum = JoRandom::Random_Int(0, 1);
		if (iRandNum)
		{
			m_eDir = LEFT;
			m_pModel->Change_Animation(Urd_StepL);
		}
		else
		{
			m_eDir = RIGHT;
			m_pModel->Change_Animation(Urd_StepR);
		}
	}
	else if (LEFT == m_eDir)
	{
		m_eDir = RIGHT;
		m_pModel->Change_Animation(Urd_StepR);
	}
	else
	{
		m_eDir = LEFT;
		m_pModel->Change_Animation(Urd_StepL);
	}
}


CUrdState_Step_LR* CUrdState_Step_LR::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_Step_LR* pInstance = new CUrdState_Step_LR(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_Step_LR"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_Step_LR::Free()
{
	__super::Free();
}
