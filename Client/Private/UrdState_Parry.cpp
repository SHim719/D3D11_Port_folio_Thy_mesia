#include "UrdState_Parry.h"

#include "Player.h"

#include "Effect_Manager.h"

CUrdState_Parry::CUrdState_Parry(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_Parry::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModel->Bind_Func("ParryToChangeSkill3", bind(&CUrdState_Parry::Check_ChangeSkill3, this));

	return S_OK;
}

void CUrdState_Parry::OnState_Start(void* pArg)
{
	m_pUrd->Set_LookTarget(false);
	m_pUrd->Set_Stanced(true);
	m_pUrd->Set_Adjust_NaviY(false);

	static_cast<CPlayer*>(m_pUrd->Get_Target())->SetState_Parried();

	_int iRandNum = JoRandom::Random_Int(0, 1);
	
	if (iRandNum)
		m_pModel->Change_Animation(Urd_Parry_L);
	else
		m_pModel->Change_Animation(Urd_Parry_R);


	EFFECTMGR->Active_Effect("Effect_Enemy_Parry_Particle", &m_pUrd->Get_EffectSpawnDesc());
}

void CUrdState_Parry::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CUrdState_Parry::Late_Update(_float fTimeDetla)
{
	if (m_pModel->Is_AnimComplete())
		__super::Decide_State();
}

void CUrdState_Parry::OnState_End()
{

}

void CUrdState_Parry::Decide_State()
{
	_int iRandNum = JoRandom::Random_Int(0, 1);
	if (0 == iRandNum)
		ADD_EVENT(bind(&CCharacter::Change_State, m_pUrd, (_uint)UrdState::State_TripleStab, nullptr));
	else
		ADD_EVENT(bind(&CCharacter::Change_State, m_pUrd, (_uint)UrdState::State_Step_B, nullptr));
}

void CUrdState_Parry::Check_ChangeSkill3()
{
	if (false == m_pUrd->Can_EnableSkill())
		return;

	_int iRandNum = JoRandom::Random_Int(0, 1);
	if (0 == iRandNum)
		ADD_EVENT(bind(&CCharacter::Change_State, m_pUrd, (_uint)UrdState::State_Skill3, nullptr));

}

CUrdState_Parry* CUrdState_Parry::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_Parry* pInstance = new CUrdState_Parry(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_Parry"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_Parry::Free()
{
	__super::Free();
}
