#include "PlayerState_Climb.h"

CPlayerState_Climb::CPlayerState_Climb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Climb::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_PossibleStates = { };

	return S_OK;
}

void CPlayerState_Climb::OnState_Start(void* pArg)
{
	m_iDir = *(_int*)pArg;

	if (KEY_PUSHING(eKeyCode::W))
	{
		if (m_iDir)
			m_pModel->Change_Animation(Corvus_SD_Ladder_Climb_R_UP);
		else
			m_pModel->Change_Animation(Corvus_SD_Ladder_Climb_L_UP);
	}
	else
	{
		if (m_iDir)
			m_pModel->Change_Animation(Corvus_SD_Ladder_Climb_L_Down);
		else
			m_pModel->Change_Animation(Corvus_SD_Ladder_Climb_R_Down);
	}
}

void CPlayerState_Climb::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos());
}

void CPlayerState_Climb::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_ClimbState(m_iDir);


}

void CPlayerState_Climb::OnState_End()
{

}

CPlayerState_Climb* CPlayerState_Climb::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Climb* pInstance = new CPlayerState_Climb(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Climb"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Climb::Free()
{
	__super::Free();
}
