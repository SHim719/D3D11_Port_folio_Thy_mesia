#include "OdurState_Hit.h"

#include "Main_Camera.h"

COdurState_Hit::COdurState_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_Hit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModel->Bind_Func("Change_HitToNextState", bind(&COdurState_Hit::Decide_State, this));

	return S_OK;
}

void COdurState_Hit::OnState_Start(void* pArg)
{
	m_pOdur->Set_LookTarget(false);
	m_pOwnerTransform->LookAt2D(m_pTargetTransform->Get_Position());

	m_pGameInstance->Play_RandomSound(L"Odur_Hurt", 1, 2, false, 1.f);
	m_pModel->Change_Animation(Magician_HurtFL + m_iHitCount % 2);
}

void COdurState_Hit::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void COdurState_Hit::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_State();
}

void COdurState_Hit::OnState_End()
{
	m_iHitCount = 0;
}

void COdurState_Hit::OnHit(const ATTACKDESC& AttackDesc)
{
	if (0 == m_pOdur->Take_Damage(AttackDesc))
	{
		m_pOdur->Change_State((_uint)OdurState::State_Stunned_Start);
		return;
	}

	m_iHitCount = (m_iHitCount + 1) % (m_iMaxHitCount + 1);
	
	_int iRandNum = JoRandom::Random_Int(0, m_iMaxHitCount);
	
	if (iRandNum <= m_iHitCount)
		m_pOdur->Change_State((_uint)OdurState::State_Parry);
	else
	{
		string strBloodEffect = m_iHitCount % 2 == 0 ? "Effect_Blood_R_Odur" : "Effect_Blood_L_Odur";
		EFFECTMGR->Active_Effect(strBloodEffect, &m_pOdur->Get_EffectSpawnDesc());
		EFFECTMGR->Active_Effect("Effect_Enemy_Hit_Particle", &m_pOdur->Get_EffectSpawnDesc());
		
		static_cast<CMain_Camera*>(GET_CAMERA)->Play_CameraShake("Shaking_Hit");
		Play_HitSound();

		OnState_Start(nullptr);
	}
		
}

COdurState_Hit* COdurState_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_Hit* pInstance = new COdurState_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_Hit::Free()
{
	__super::Free();
}
