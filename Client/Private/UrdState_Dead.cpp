#include "UrdState_Dead.h"

#include "Player.h"

#include "Effect_Manager.h"

CUrdState_Dead::CUrdState_Dead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_Dead::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUrdState_Dead::OnState_Start(void* pArg)
{
	m_pUrd->Set_LookTarget(false);
	m_pUrd->Set_Stanced(true);

	PLAY_SOUND(L"Urd_Dead", false, 1.f);
	m_pModel->Change_Animation(Urd_Dead, 0.f, false);
}

void CUrdState_Dead::Update(_float fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CUrdState_Dead::Late_Update(_float fTimeDetla)
{
	if (true == m_pModel->Is_AnimComplete() && false == m_pUrd->Is_Dissolving())
		m_pUrd->Active_Dissolve();
}

void CUrdState_Dead::OnState_End()
{

}

CUrdState_Dead* CUrdState_Dead::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_Dead* pInstance = new CUrdState_Dead(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_Dead"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_Dead::Free()
{
	__super::Free();
}
