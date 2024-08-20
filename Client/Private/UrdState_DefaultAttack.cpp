#include "UrdState_DefaultAttack.h"

#include "Effect_Manager.h"

CUrdState_DefaultAttack::CUrdState_DefaultAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_DefaultAttack::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Init_AttackDesc();

	return S_OK;
}

void CUrdState_DefaultAttack::OnState_Start(void* pArg)
{
	m_pUrd->Set_LookTarget(true);
	m_pUrd->Set_Stanced(true);

	Reset_AttackIdx();
	m_pUrd->Update_AttackDesc();

	m_pModel->Change_Animation(Urd_Attack05);

	PLAY_SOUND(L"Urd_DefaultAttack", false, 0.3f);
}

void CUrdState_DefaultAttack::Update(_float fTimeDelta)
{
	if (false == m_pUrd->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CUrdState_DefaultAttack::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		__super::Decide_State();
}


void CUrdState_DefaultAttack::OnState_End()
{

}

void CUrdState_DefaultAttack::Init_AttackDesc()
{
	ATTACKDESC AttackDesc{};
	AttackDesc.eEnemyAttackType = NORMAL;
	AttackDesc.iDamage = 73;

	m_AttackDescs.emplace_back(make_pair(CUrd::SWORD, AttackDesc));

	AttackDesc.eEnemyAttackType = SEMIKNOCKBACK;
	AttackDesc.iDamage = 87;
	AttackDesc.strShakingTag = "Shaking_Big_Hit";

	m_AttackDescs.emplace_back(make_pair(CUrd::SWORD, AttackDesc));
}

void CUrdState_DefaultAttack::Decide_State()
{
	_int iRandNum = 0;

	if (m_pUrd->Is_Phase2())
	{
		iRandNum = JoRandom::Random_Int(0, 3);
		if (0 == iRandNum)
			return;
		else
		{
			iRandNum = JoRandom::Random_Int(0, 5);
			switch (iRandNum)
			{
			case 0:
				ADD_EVENT(bind(&CCharacter::Change_State, m_pUrd, (_uint)State_ExtraAttack, nullptr));
				break;
			case 1:
				ADD_EVENT(bind(&CCharacter::Change_State, m_pUrd, (_uint)State_Skill2, nullptr));
				break;
			default:
				ADD_EVENT(bind(&CUrdState_Base::Decide_Step, this));
				break;
			}
		}
	}
	else
	{
		iRandNum = JoRandom::Random_Int(0, 1);
		if (1 == iRandNum)
			ADD_EVENT(bind(&CUrdState_Base::Decide_Step, this));
	}
}

CUrdState_DefaultAttack* CUrdState_DefaultAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_DefaultAttack* pInstance = new CUrdState_DefaultAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_DefaultAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_DefaultAttack::Free()
{
	__super::Free();
}
