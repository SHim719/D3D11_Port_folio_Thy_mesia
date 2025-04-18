#include "UrdState_TripleStab.h"

#include "Effect_Manager.h"

CUrdState_TripleStab::CUrdState_TripleStab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_TripleStab::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Init_AttackDesc();

	m_pModel->Bind_Func("TrippleStab_OneMore", bind(&CUrdState_TripleStab::Attack_OneMore, this));

	return S_OK;
}

void CUrdState_TripleStab::OnState_Start(void* pArg)
{
	m_pUrd->Set_LookTarget(true);
	m_pUrd->Set_Stanced(true);

	Reset_AttackIdx();
	m_pUrd->Update_AttackDesc();

	if ((_uint)UrdState::State_Parry == m_pUrd->Get_PrevState())
	{
		m_pModel->Change_AnimationWithStartFrame(Urd_Attack01LV2, 18, 0.05f, true);
		PLAY_SOUND(L"Urd_Attack1", false, 1.f);
		return;
	}
	else
		m_pModel->Change_Animation(Urd_Attack01);


	if (m_pUrd->Is_Phase2())
		PLAY_SOUND(L"Urd_Attack1_P2", false, 1.f);
	else
		PLAY_SOUND(L"Urd_Attack1", false, 1.f);
}

void CUrdState_TripleStab::Update(_float fTimeDelta)
{
	if (false == m_pUrd->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CUrdState_TripleStab::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pUrd->Change_State((_uint)UrdState::State_Walk);
}


void CUrdState_TripleStab::OnState_End()
{
	if (m_pGameInstance->Is_Playing(L"Urd_Attack1"))
	{
		m_pGameInstance->Stop(L"Urd_Attack1");
		return;
	}
		
	if (m_pGameInstance->Is_Playing(L"Urd_Attack1_P2"))
		m_pGameInstance->Stop(L"Urd_Attack1_P2");

	
}

void CUrdState_TripleStab::Init_AttackDesc()
{
	ATTACKDESC AttackDesc;
	AttackDesc.eEnemyAttackType = NORMAL;
	AttackDesc.iDamage = 64;

	m_AttackDescs.emplace_back(make_pair(CUrd::SWORD, AttackDesc));
}

void CUrdState_TripleStab::Decide_State()
{
	_int iRandNum = JoRandom::Random_Int(0, 5);
	
	switch (iRandNum)
	{
	case 0:
		if (m_pUrd->Can_EnableSkill())
			ADD_EVENT(bind(&CCharacter::Change_State, m_pUrd, (_uint)State_Skill2, nullptr));
		else
			ADD_EVENT(bind(&CCharacter::Change_State, m_pUrd, (_uint)State_ExtraAttack, nullptr));
		break;
	case 1:
		ADD_EVENT(bind(&CCharacter::Change_State, m_pUrd, (_uint)State_ExtraAttack, nullptr));
		break;
	default:
		ADD_EVENT(bind(&CUrdState_Base::Decide_Step, this));
		break;
	}
}


void CUrdState_TripleStab::Attack_OneMore()
{
	// 특정프레임에 시작
	if (m_pUrd->Is_Phase2())
		ADD_EVENT(bind(&CModel::Change_AnimationWithStartFrame, m_pModel, Urd_Attack01LV2, 18, 0.05f, true));
}



CUrdState_TripleStab* CUrdState_TripleStab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_TripleStab* pInstance = new CUrdState_TripleStab(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_TripleStab"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_TripleStab::Free()
{
	__super::Free();
}
