#include "PerceptionBounding.h"

#include "Enemy.h"

CPerceptionBounding::CPerceptionBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPerceptionBounding::CPerceptionBounding(const CPerceptionBounding& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPerceptionBounding::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPerceptionBounding::Initialize(void* pArg)
{
	m_pOwner = (CEnemy*)pArg;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransform->Set_Position(m_pOwner->Get_Transform()->Get_Position());
	m_pPerceptionCollider->Update(m_pTransform->Get_WorldMatrix());

	return S_OK;
}

void CPerceptionBounding::Tick(_float fTimeDelta)
{

}

void CPerceptionBounding::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPerceptionBounding::Render()
{
	if (m_pPerceptionCollider)
		m_pPerceptionCollider->Render();

	return S_OK;
}

void CPerceptionBounding::OnCollisionEnter(CGameObject* pOther)
{
	ADD_EVENT(bind(&CGameObject::Set_Destroy, this, true));
	m_pOwner->Percept_Target();
}

HRESULT CPerceptionBounding::Ready_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc = {};
	ColliderDesc.bActive = true;
	ColliderDesc.eType = CCollider::SPHERE;
	ColliderDesc.pOwner = this;
	ColliderDesc.strCollisionLayer = "PerceptionBounding";
	ColliderDesc.vCenter = { 0.f, 0.3f, 0.f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };
	ColliderDesc.vSize = { 6.f, 0.f, 0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("Collider"), (CComponent**)&m_pPerceptionCollider, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CPerceptionBounding* CPerceptionBounding::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPerceptionBounding* pInstance = new CPerceptionBounding(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPerceptionBounding"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPerceptionBounding::Clone(void* pArg)
{
	CPerceptionBounding* pInstance = new CPerceptionBounding(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPerceptionBounding"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPerceptionBounding::Free()
{
	__super::Free();

	Safe_Release(m_pPerceptionCollider);

	m_pOwner = nullptr;
}
