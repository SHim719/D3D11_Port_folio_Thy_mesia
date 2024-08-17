#include "UrdState_Idle.h"

CUrdState_Idle::CUrdState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_Idle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUrdState_Idle::OnState_Start(void* pArg)
{
	m_pModel->Change_Animation(Urd_Idle);
}

void CUrdState_Idle::Update(_float fTimeDelta)
{
}

void CUrdState_Idle::OnState_End()
{

}

void CUrdState_Idle::OnHit(const ATTACKDESC& AttackDesc)
{

}

CUrdState_Idle* CUrdState_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_Idle* pInstance = new CUrdState_Idle(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_Idle::Free()
{
	__super::Free();
}

