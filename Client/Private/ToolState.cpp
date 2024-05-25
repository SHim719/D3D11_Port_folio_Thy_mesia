#include "ToolState.h"

CToolState::CToolState(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice)
    , m_pContext(pContext)
    , m_pGameInstance(CGameInstance::Get_Instance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CToolState::Initialize(void* pArg)
{
    m_pCamera = static_cast<CFree_Camera*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, L"Camera"));

	return S_OK;
}

void CToolState::Tick(_float fTimeDelta)
{
}

void CToolState::Free()
{
	__super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
