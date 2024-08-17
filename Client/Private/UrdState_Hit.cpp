#include "UrdState_Hit.h"

CUrdState_Hit::CUrdState_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_Hit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModel->Bind_Func("Change_HitToNextState", bind(&CUrdState_Hit::Decide_State, this));

	return S_OK;
}

void CUrdState_Hit::OnState_Start(void* pArg)
{
	string strBloodEffect = m_iHitCount % 2 == 0 ? "Effect_Blood_R_Odur" : "Effect_Blood_L_Odur";
	EFFECTMGR->Active_Effect(strBloodEffect, &m_pUrd->Get_EffectSpawnDesc());
	EFFECTMGR->Active_Effect("Effect_Enemy_Hit_Particle", &m_pUrd->Get_EffectSpawnDesc());

	m_pModel->Change_Animation(Urd_HurtM_FL + m_iHitCount % 2);
}

void CUrdState_Hit::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CUrdState_Hit::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_State();
}

void CUrdState_Hit::OnState_End()
{
	m_iHitCount = 0;
}

void CUrdState_Hit::OnHit(const ATTACKDESC& AttackDesc)
{
	m_iHitCount = (m_iHitCount + 1) % (m_iMaxHitCount + 1);

	_int iRandNum = JoRandom::Random_Int(0, m_iMaxHitCount);

	if (iRandNum <= m_iHitCount)
		m_pUrd->Change_State((_uint)UrdState::State_Parry);
	else
		OnState_Start(nullptr);
}

CUrdState_Hit* CUrdState_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_Hit* pInstance = new CUrdState_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_Hit::Free()
{
	__super::Free();
}
