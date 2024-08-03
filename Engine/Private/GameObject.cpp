#include "..\Public\GameObject.h"
#include "GameInstance.h"




CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice{ rhs.m_pDevice }
	, m_pContext{rhs.m_pContext }
	, m_pGameInstance{ rhs.m_pGameInstance }
	, m_bDestroyed { false }
	, m_fDissolveAmount { rhs.m_fDissolveAmount }
	, m_fDissolveSpeed { rhs.m_fDissolveSpeed }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{

	return S_OK;
}

void CGameObject::OnActive()
{
}

void CGameObject::OnInActive()
{
}

HRESULT CGameObject::OnEnter_Layer(void* pArg)
{
	m_bReturnToPool = false;

	return S_OK;
}

void CGameObject::PriorityTick(_float fTimeDelta)
{
}

void CGameObject::Tick(_float fTimeDelta)
{
}

void CGameObject::LateTick(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}


HRESULT CGameObject::Bind_ShaderResources()
{
	return S_OK;
}

void CGameObject::Decide_PassIdx()
{

}

void CGameObject::Active_Dissolve()
{
	m_bDissolve = true;
	m_fDissolveAmount = m_fDissolveSpeed < 0.f ? 1.f : 0.f;

	Decide_PassIdx();
}

void CGameObject::Update_Dissolve(_float fTimeDelta)
{
	m_fDissolveAmount = clamp(m_fDissolveAmount + m_fDissolveSpeed * fTimeDelta, 0.f, 1.f);

	if (0.f == m_fDissolveAmount || 1.f == m_fDissolveAmount)
	{
		m_bDissolve = false;
		Decide_PassIdx();
	}
}

HRESULT CGameObject::Add_Component(_uint iPrototoypeLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, CComponent** ppOut, void* pArg)
{
	if (nullptr != Find_Component(strComponentTag))
		return E_FAIL;

	CComponent* pComponent = m_pGameInstance->Clone_Component(iPrototoypeLevelIndex, strPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(strComponentTag, pComponent);

	if (nullptr != ppOut)
	{
		*ppOut = pComponent;
		Safe_AddRef(pComponent);
	}

	return S_OK;
}

CComponent* CGameObject::Find_Component(const wstring& strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag);

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}


void CGameObject::Set_Active(_bool b)
{
	m_bActive = b;
	if (m_bActive)
		OnActive();
	else
		OnInActive();
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	Safe_Release(m_pTransform);
	Safe_Release(m_pDissolveTexture);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
