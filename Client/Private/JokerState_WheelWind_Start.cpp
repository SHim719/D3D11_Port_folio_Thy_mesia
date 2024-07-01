#include "JokerState_WheelWind_Start.h"

CJokerState_WheelWind_Start::CJokerState_WheelWind_Start(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CJokerState_Base(pDevice, pContext)
{
}

HRESULT CJokerState_WheelWind_Start::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CJokerState_WheelWind_Start::OnState_Start(void* pArg)
{
	Reset_AttackIdx();
	m_pJoker->Set_LookTarget(true);

	m_pModel->Change_Animation(Joker_WheelAttackStart);
}

void CJokerState_WheelWind_Start::Update(_float fTimeDelta)
{
	if (!m_pJoker->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CJokerState_WheelWind_Start::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pJoker->Change_State((_uint)JokerState::State_WheelWind_Loop);
}

void CJokerState_WheelWind_Start::OnState_End()
{

}

CJokerState_WheelWind_Start* CJokerState_WheelWind_Start::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CJokerState_WheelWind_Start* pInstance = new CJokerState_WheelWind_Start(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CJokerState_WheelWind_Start"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJokerState_WheelWind_Start::Free()
{
	__super::Free();
}
