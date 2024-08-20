#include "UrdState_Ultimate.h"

CUrdState_Ultimate::CUrdState_Ultimate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_Ultimate::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	return S_OK;
}

void CUrdState_Ultimate::OnState_Start(void* pArg)
{
	PLAY_SOUND(L"Urd_SP1", false, 1.f);

	m_pModel->Change_Animation(Urd_SPSkill01);
}

void CUrdState_Ultimate::Update(_float fTimeDelta)
{
	

}

void CUrdState_Ultimate::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pUrd->Change_State((_uint)UrdState::State_Walk);

}

void CUrdState_Ultimate::OnState_End()
{

}


CUrdState_Ultimate* CUrdState_Ultimate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_Ultimate* pInstance = new CUrdState_Ultimate(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_Ultimate"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_Ultimate::Free()
{
	__super::Free();
}

