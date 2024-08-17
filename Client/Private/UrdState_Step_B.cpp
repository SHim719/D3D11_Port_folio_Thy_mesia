#include "UrdState_Step_B.h"

#include "Effect_Manager.h"

CUrdState_Step_B::CUrdState_Step_B(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_Step_B::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUrdState_Step_B::OnState_Start(void* pArg)
{
	m_pUrd->Set_LookTarget(false);
	m_pUrd->Set_Stanced(true);
	m_pUrd->Set_Invincible(true);

	EFFECTMGR->Active_Effect("Effect_Urd_Step_Particle", &m_pUrd->Get_EffectSpawnDesc());

}

void CUrdState_Step_B::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CUrdState_Step_B::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		__super::Decide_State();
}


void CUrdState_Step_B::OnState_End()
{
	m_pUrd->Set_Invincible(false);
}



CUrdState_Step_B* CUrdState_Step_B::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_Step_B* pInstance = new CUrdState_Step_B(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_Step_B"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_Step_B::Free()
{
	__super::Free();
}
