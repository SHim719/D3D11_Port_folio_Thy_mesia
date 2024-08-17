#include "OdurState_Stunned_Loop.h"

#include "Player.h"

COdurState_Stunned_Loop::COdurState_Stunned_Loop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_Stunned_Loop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void COdurState_Stunned_Loop::OnState_Start(void* pArg)
{
	static_cast<CPlayer*>(m_pOdur->Get_Target())->Add_StunnedEnemy(m_pOdur);

	m_pModel->Change_Animation(Magician_StunLoop_Cane);
}

void COdurState_Stunned_Loop::Update(_float fTimeDelta)
{

}

void COdurState_Stunned_Loop::Late_Update(_float fTimeDelta)
{


}

void COdurState_Stunned_Loop::OnState_End()
{
}

void COdurState_Stunned_Loop::OnHit(const ATTACKDESC& AttackDesc)
{

}

COdurState_Stunned_Loop* COdurState_Stunned_Loop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_Stunned_Loop* pInstance = new COdurState_Stunned_Loop(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_Stunned_Loop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_Stunned_Loop::Free()
{
	__super::Free();
}
