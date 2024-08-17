#include "UrdState_Skill3.h"

CUrdState_Skill3::CUrdState_Skill3(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_Skill3::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUrdState_Skill3::OnState_Start(void* pArg)
{
	m_pUrd->Set_LookTarget(true);
	m_pUrd->Set_Stanced(true);

	m_pModel->Change_Animation(Urd_Skill03_L);
	m_pUrd->Active_UrdWeapon();
}

void CUrdState_Skill3::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CUrdState_Skill3::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		__super::Decide_State();

}

void CUrdState_Skill3::OnState_End()
{

}


CUrdState_Skill3* CUrdState_Skill3::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_Skill3* pInstance = new CUrdState_Skill3(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_Skill3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_Skill3::Free()
{
	__super::Free();
}

