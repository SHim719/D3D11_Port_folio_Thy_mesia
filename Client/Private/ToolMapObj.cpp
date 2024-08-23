#include "ToolMapObj.h"
#include "Map_Tool.h"

CToolMapObj::CToolMapObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CToolMapObj::CToolMapObj(const CToolMapObj& rhs)
	: CGameObject(rhs)
	, m_eObjType(rhs.m_eObjType)
	, m_iNaviIdx(rhs.m_iNaviIdx)
	, m_iTriggerIdx(rhs.m_iTriggerIdx)
{
	if (rhs.m_pShader)
		m_pShader = static_cast<CShader*>(rhs.m_pShader->Clone(nullptr));

	if (rhs.m_pModel)
		m_pModel = static_cast<CModel*>(rhs.m_pModel->Clone(nullptr));

	if (rhs.m_pTriggerCollider)
		m_pTriggerCollider = static_cast<CCollider*>(rhs.m_pTriggerCollider->Clone(nullptr));

	if (rhs.m_pTransform)
	{
		m_pTransform = static_cast<CTransform*>(rhs.m_pTransform->Clone(nullptr));
		m_pTransform->Set_WorldMatrix(rhs.m_pTransform->Get_WorldMatrix());
	}
}	

HRESULT CToolMapObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CToolMapObj::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	TOOLMAPOBJDESC* pDesc = (TOOLMAPOBJDESC*)pArg;

	m_eObjType = pDesc->eObjType;

	if (FAILED(Ready_Components(pDesc->wstrModelTag)))
		return E_FAIL;


	return S_OK;
}

HRESULT CToolMapObj::Initialize_Load(void* pArg)
{
	LOADOBJDESC* pDesc = (LOADOBJDESC*)pArg;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pDesc->WorldMatrix));

	m_eObjType = pDesc->eObjType;
	m_iNaviIdx = pDesc->iNaviIdx;
	m_iTriggerIdx = pDesc->iTriggerIdx;
	m_vColliderSize = pDesc->vColliderSize;

	if (m_eObjType < TRIGGEROBJ)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModel"), TEXT("Shader"), (CComponent**)&m_pShader)))
			return E_FAIL;

		if (FAILED(__super::Add_Component(LEVEL_TOOL, pDesc->szModelTag, L"Model", (CComponent**)&m_pModel)))
			return E_FAIL;
	}
	else
	{
		CCollider::COLLIDERDESC CollisionDesc;
		CollisionDesc.eType = CCollider::AABB;
		CollisionDesc.pOwner = this;
		CollisionDesc.vCenter = { 0.f, 0.f, 0.f };
		CollisionDesc.vSize = pDesc->vColliderSize;
		CollisionDesc.vRotation = { 0.f, 0.f, 0.f };
		CollisionDesc.bActive = true;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_AABB", L"Collider", (CComponent**)&m_pTriggerCollider, &CollisionDesc)))
			return E_FAIL;
	}

	return S_OK;
}

void CToolMapObj::Tick(_float fTimeDelta)
{

}

void CToolMapObj::LateTick(_float fTimeDelta)
{
	if (m_pTriggerCollider)
	{
		m_pTriggerCollider->Update(m_pTransform->Get_WorldMatrix());

		m_pGameInstance->Add_RenderComponent(m_pTriggerCollider);
	
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	}
}

HRESULT CToolMapObj::Render()
{

	return S_OK;
}

HRESULT CToolMapObj::Render_Picking(_int iSelectIdx)
{
	if (nullptr == m_pShader || nullptr == m_pModel)
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;


	float arrPickingColor[4] = {};
	_float4 vPickingColor = { 0.f, 0.f, 0.f, 0.f };

	for (_uint i = 0; i < 4; ++i)
	{
		if (iSelectIdx < 0)
			break;

		arrPickingColor[i] = ((_float)iSelectIdx / 255) >= 1 ? 1.f : (_float)iSelectIdx / 255;
		iSelectIdx -= 255;
	}

	vPickingColor.x = arrPickingColor[0];
	vPickingColor.y = arrPickingColor[1];
	vPickingColor.z = arrPickingColor[2];
	vPickingColor.w = arrPickingColor[3];

	if (FAILED(m_pShader->Set_RawValue("g_vPickingColor", &vPickingColor, sizeof(_float4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, 1)))
			return E_FAIL;
	}

	return S_OK;
}

_bool CToolMapObj::Ray_Cast(_fvector vRayStartPos, _fvector vRayDir, OUT _float4& vPickedPos, OUT _float& fDist)
{
	return m_pModel->Picking(m_pTransform->Get_WorldMatrix(), vRayStartPos, vRayDir, vPickedPos, fDist);
}

_float3 CToolMapObj::Get_ColliderSize() const
{
	if (m_pTriggerCollider)
	{
		_float3 vSize;
		XMStoreFloat3(&vSize, m_pTriggerCollider->Get_Size());
		return vSize;
	}
	return m_vColliderSize;
}

void CToolMapObj::Set_ColliderSize(const _float3& vSize)
{
	m_vColliderSize = vSize;
	if (m_pTriggerCollider)
		m_pTriggerCollider->Set_Size(XMLoadFloat3(&vSize));
}

HRESULT CToolMapObj::Ready_Components(const wstring& wstrModelTag)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModel"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (m_eObjType < TRIGGEROBJ)
	{
		if (FAILED(__super::Add_Component(LEVEL_TOOL, wstrModelTag, L"Model", (CComponent**)&m_pModel)))
			return E_FAIL;
	}
	else
	{
		CCollider::COLLIDERDESC CollisionDesc;
		CollisionDesc.eType = CCollider::AABB;
		CollisionDesc.pOwner = this;
		CollisionDesc.vCenter = { 0.f, 0.f, 0.f };
		CollisionDesc.vSize = { 1.f, 1.f, 1.f };
		CollisionDesc.vRotation = { 0.f, 0.f, 0.f };
		CollisionDesc.bActive = true;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_AABB", L"Collider", (CComponent**)&m_pTriggerCollider, &CollisionDesc)))
			return E_FAIL; 
	}
	
	return S_OK;
}


CToolMapObj* CToolMapObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolMapObj* pInstance = new CToolMapObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CToolMapObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToolMapObj::Clone(void* pArg)
{
	CToolMapObj* pInstance = new CToolMapObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CToolMapObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolMapObj::Free()
{
	__super::Free();

	Safe_Release(m_pModel);
	Safe_Release(m_pShader);
	Safe_Release(m_pTriggerCollider);
}
