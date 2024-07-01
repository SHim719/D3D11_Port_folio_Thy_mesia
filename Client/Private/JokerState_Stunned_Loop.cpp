#include "JokerState_Stunned_Loop.h"
#include "Player.h"

CJokerState_Stunned_Loop::CJokerState_Stunned_Loop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CJokerState_Base(pDevice, pContext)
{
}

HRESULT CJokerState_Stunned_Loop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CJokerState_Stunned_Loop::OnState_Start(void* pArg)
{
	m_pJoker->Set_LookTarget(false);

	static_cast<CPlayer*>(m_pJoker->Get_Target())->Add_StunnedEnemy(m_pJoker);

	m_pModel->Change_Animation(Joker_Stun_Loop);
}

void CJokerState_Stunned_Loop::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CJokerState_Stunned_Loop::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pJoker->Change_State((_uint)JokerState::State_Stunned_Loop);

}

void CJokerState_Stunned_Loop::OnState_End()
{
}

void CJokerState_Stunned_Loop::OnHit(const ATTACKDESC& AttackDesc)
{

}

CJokerState_Stunned_Loop* CJokerState_Stunned_Loop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CJokerState_Stunned_Loop* pInstance = new CJokerState_Stunned_Loop(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CJokerState_Stunned_Loop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJokerState_Stunned_Loop::Free()
{
	__super::Free();
}
