#include "UrdState_Stunned_Loop.h"

#include "Player.h"

CUrdState_Stunned_Loop::CUrdState_Stunned_Loop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_Stunned_Loop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUrdState_Stunned_Loop::OnState_Start(void* pArg)
{
	static_cast<CPlayer*>(m_pUrd->Get_Target())->Add_StunnedEnemy(m_pUrd);

	m_pModel->Change_Animation(Urd_StunLoop);
}

void CUrdState_Stunned_Loop::Update(_float fTimeDelta)
{

}

void CUrdState_Stunned_Loop::Late_Update(_float fTimeDelta)
{


}

void CUrdState_Stunned_Loop::OnState_End()
{
}

void CUrdState_Stunned_Loop::OnHit(const ATTACKDESC& AttackDesc)
{

}

CUrdState_Stunned_Loop* CUrdState_Stunned_Loop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_Stunned_Loop* pInstance = new CUrdState_Stunned_Loop(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_Stunned_Loop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_Stunned_Loop::Free()
{
	__super::Free();
}
