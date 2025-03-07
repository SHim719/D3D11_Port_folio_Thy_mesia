#include "Weapon.h"

#include "Bone.h"

#include "Effect_Trail.h"


CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CWeapon::CWeapon(const CWeapon& rhs)
	: CGameObject(rhs)
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon::Initialize(void* pArg)
{
	WEAPONDESC* pWeaponDesc = (WEAPONDESC*)pArg;

	if (FAILED(Ready_Components(pWeaponDesc)))
		return E_FAIL;

	m_iTag = pWeaponDesc->iTag;
	m_bAlphaEnabled = pWeaponDesc->bAlphaBlend;

	m_pOwner = pWeaponDesc->pOwner;
	m_pSocketBone = pWeaponDesc->pSocketBone;
	m_pParentTransform = pWeaponDesc->pParentTransform;

	Safe_AddRef(m_pSocketBone);
	Safe_AddRef(m_pParentTransform);

	if (pWeaponDesc->bTrail)
		m_pEffect_Trail = static_cast<CEffect_Trail*>(m_pGameInstance->Clone_GameObject(pWeaponDesc->wstrTrailTag));
		
	return S_OK;
}

void CWeapon::Tick(_float fTimeDelta)
{
	_matrix SocketMatrix = m_pSocketBone->Get_CombinedTransformation();
	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	m_pTransform->Set_WorldMatrix(SocketMatrix * m_pParentTransform->Get_WorldMatrix());

	if (m_bTrailActivated)
	{
		m_pEffect_Trail->Update_ParentMatrix(m_pTransform->Get_WorldMatrix());
		m_pEffect_Trail->Tick(fTimeDelta);
	}
		
	if (m_pCollider)
		m_pCollider->Update(m_pTransform->Get_WorldMatrix());
}

void CWeapon::LateTick(_float fTimeDelta)
{
	if (m_bNoRender || nullptr != m_pOwner && m_pOwner->No_Render())
		return;

#ifdef _DEBUG
	m_pGameInstance->Add_RenderComponent(m_pCollider);
#endif

	if (nullptr == m_pModel)
		return;

	if (m_bTrailActivated)
		m_pEffect_Trail->LateTick(fTimeDelta);

	if (true == m_bAlphaEnabled && m_fAlpha < 0.5f)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	else
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	
}

HRESULT CWeapon::Render()
{
	_uint iPassIndex = 0;
		
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (m_bAlphaEnabled && m_fAlpha < 0.5f)
	{
		if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
			return E_FAIL;

		iPassIndex = 2;
	}
		
	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;

		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, iPassIndex)))
			return E_FAIL;
	}
	return S_OK;
}


void CWeapon::Set_Active_Trail(_bool bActive)
{
	m_bTrailActivated = bActive;
	if (true == bActive)
	{
		m_pEffect_Trail->Reset_TrailBuffer();
		m_pEffect_Trail->Reset_TrailAcc();
		m_pEffect_Trail->Update_ParentMatrix(m_pTransform->Get_WorldMatrix());
	}
}

HRESULT CWeapon::Ready_Components(WEAPONDESC* pDesc)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform, nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModel"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Attackable"), TEXT("Attackable"), (CComponent**)&m_pAttackable)))
		return E_FAIL;

	if (pDesc->wstrModelTag.size())
	{
		if (FAILED(__super::Add_Component(pDesc->iLevelID, pDesc->wstrModelTag, TEXT("Model"), (CComponent**)&m_pModel)))
			return E_FAIL;
	}
	
	if (nullptr != pDesc->pColliderDesc)
	{
		wstring wstrColliderTag = L"";
		if (CCollider::OBB == pDesc->pColliderDesc->eType)
			wstrColliderTag = L"Prototype_OBB";
		else if (CCollider::SPHERE == pDesc->pColliderDesc->eType)
			wstrColliderTag = L"Prototype_Sphere";

		pDesc->pColliderDesc->pOwner = this;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, wstrColliderTag, TEXT("Collider"), (CComponent**)&m_pCollider,
			pDesc->pColliderDesc)))
			return E_FAIL;
	}

	return S_OK;
}

CWeapon* CWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon* pInstance = new CWeapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CWeapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon::Clone(void* pArg)
{
	CWeapon* pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CWeapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon::Free()
{
	__super::Free();

	Safe_Release(m_pSocketBone);
	Safe_Release(m_pParentTransform);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pCollider);
	Safe_Release(m_pEffect_Trail);
	Safe_Release(m_pAttackable);
}
