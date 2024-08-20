#include "UrdState_Skill2.h"

CUrdState_Skill2::CUrdState_Skill2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_Skill2::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUrdState_Skill2::OnState_Start(void* pArg)
{
	m_pOwnerTransform->LookAt2D(m_pTargetTransform->Get_Position());
	m_pUrd->Set_Stanced(true);
	m_pUrd->Set_LookTarget(false);
	m_pUrd->Set_Adjust_NaviY(false);

	m_pModel->Change_Animation(Urd_Skill02_1);
	m_pUrd->Active_UrdWeapon();

	PLAY_SOUND(L"Urd_Skill_Action", false, 1.f);
}

void CUrdState_Skill2::Update(_float fTimeDelta)
{
	if (false == m_pUrd->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CUrdState_Skill2::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		__super::Decide_State();
}

void CUrdState_Skill2::OnState_End()
{
	m_pUrd->Set_Adjust_NaviY(true);
}


CUrdState_Skill2* CUrdState_Skill2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_Skill2* pInstance = new CUrdState_Skill2(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_Skill2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_Skill2::Free()
{
	__super::Free();
}

