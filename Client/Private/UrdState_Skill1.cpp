#include "UrdState_Skill1.h"

CUrdState_Skill1::CUrdState_Skill1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_Skill1::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUrdState_Skill1::OnState_Start(void* pArg)
{
	m_pOwnerTransform->LookAt2D(m_pTargetTransform->Get_Position());
	m_pUrd->Set_LookTarget(false);
	m_pUrd->Set_Stanced(true);
	m_pUrd->Set_Adjust_NaviY(false);

	m_pUrd->Active_UrdWeapon();
	m_pModel->Change_Animation(Urd_Skill01);

	PLAY_SOUND(L"Urd_Skill_Action", false, 0.7f);
}

void CUrdState_Skill1::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CUrdState_Skill1::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		__super::Decide_State();

}

void CUrdState_Skill1::OnState_End()
{
	m_pUrd->Set_Adjust_NaviY(true);
}


CUrdState_Skill1* CUrdState_Skill1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_Skill1* pInstance = new CUrdState_Skill1(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_Skill1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_Skill1::Free()
{
	__super::Free();
}

