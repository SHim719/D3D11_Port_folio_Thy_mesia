#include "OdurState_Base.h"

#include "GameObject.h"

#include "Main_Camera.h"

COdurState_Base::COdurState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CState_Base(pDevice, pContext)
{
}

HRESULT COdurState_Base::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pOdur = (COdur*)pArg;

	m_pTargetTransform = m_pOdur->Get_Target()->Get_Transform();
	Safe_AddRef(m_pTargetTransform);

	Init_AttackDesc();

	return S_OK;
}

void COdurState_Base::OnState_Start(void* pArg)
{
}

void COdurState_Base::Update(_float fTimeDelta)
{

}

void COdurState_Base::OnState_End()
{
}

void COdurState_Base::OnHit(const ATTACKDESC& AttackDesc)
{
	if (0 == m_pOdur->Take_Damage(AttackDesc))
		int x = 10; // ���� ó��
	else if (!m_pOdur->Is_Stanced())
	{
		m_pOdur->Change_State((_uint)OdurState::State_Hit);
		return;
	}
		

	_int iRandNum = JoRandom::Random_Int(0, 1);
	string strBloodEffect = iRandNum == 0 ? "Effect_Blood_R_Odur" : "Effect_Blood_L_Odur";
	EFFECTMGR->Active_Effect(strBloodEffect, &m_pOdur->Get_EffectSpawnDesc());
	EFFECTMGR->Active_Effect("Effect_Enemy_Hit_Particle", &m_pOdur->Get_EffectSpawnDesc());

	static_cast<CMain_Camera*>(GET_CAMERA)->Play_CameraShake("Shaking_Hit");
}

void COdurState_Base::Decide_State()
{
	_int iRandNum = JoRandom::Random_Int(0, 9);

	if (iRandNum < 5)
		Decide_Attack();
	else if (iRandNum < 8)
		m_pOdur->Change_State((_uint)OdurState::State_DisappearMove);
	else
		m_pOdur->Change_State((_uint)OdurState::State_Walk);
}

void COdurState_Base::Decide_Attack()
{
	_int 	iRandNum = JoRandom::Random_Int(0, 2);

	if (0 == iRandNum)
		m_pOdur->Change_State((_uint)OdurState::State_CaneAttack1);
	else if (1 == iRandNum)
		m_pOdur->Change_State((_uint)OdurState::State_CaneAttack2);
	else
		m_pOdur->Change_State((_uint)OdurState::State_KickCombo);
}

void COdurState_Base::Free()
{
	__super::Free();

	Safe_Release(m_pTargetTransform);
}
