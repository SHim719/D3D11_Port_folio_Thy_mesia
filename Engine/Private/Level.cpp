#include "Level.h"

#include "GameInstance.h"
#include "GameObject.h"

CLevel::CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}


HRESULT CLevel::Initialize()
{

	return S_OK;
}


void CLevel::PriorityTick(_float fTimeDelta)
{
}

void CLevel::Tick(_float fTimeDelta)
{
}

void CLevel::LateTick(_float fTimeDelta)
{
}

HRESULT CLevel::Render()
{
	return S_OK;
}

void CLevel::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
