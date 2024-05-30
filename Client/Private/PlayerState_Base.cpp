#include "PlayerState_Base.h"


CPlayerState_Base::CPlayerState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Base::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pPlayer = (CPlayer*)pArg;

	m_pMain_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Camera", 0));
	if (nullptr == m_pMain_Camera)
		return E_FAIL;

	return S_OK;
}

void CPlayerState_Base::OnState_Start()
{
}

void CPlayerState_Base::OnGoing(_float fTimeDelta)
{
}

void CPlayerState_Base::OnState_End()
{
}

void CPlayerState_Base::Free()
{
	__super::Free();

}
