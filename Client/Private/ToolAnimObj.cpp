#include "ToolAnimObj.h"
#include "Weapon.h"


CToolAnimObj::CToolAnimObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CToolAnimObj::CToolAnimObj(const CToolAnimObj& rhs)
	: CGameObject(rhs)
{
}

HRESULT CToolAnimObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CToolAnimObj::Initialize(void* pArg)
{
	PrototypeCharacter* pType = (PrototypeCharacter*)pArg;

	switch (*pType)
	{
	case Corvus:
		if (FAILED(Ready_Corvus()))
			return E_FAIL;
		break;
	}


	m_pModel->Set_Preview(true);
	return S_OK;
}

void CToolAnimObj::Tick(_float fTimeDelta)
{

}

void CToolAnimObj::LateTick(_float fTimeDelta)
{
	m_pModel->Play_Animation(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CToolAnimObj::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pModel->SetUp_BoneMatrices(m_pShader)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint j = 0; j < iNumMeshes; ++j)
	{
		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, j, TextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModel->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/

		if (FAILED(m_pModel->Render(m_pShader, j, 0)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CToolAnimObj::Ready_Corvus()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_TOOL, L"Prototype_Model_Player", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	CWeapon::WEAPONDESC WeaponDesc;
	WeaponDesc.pParentTransform = m_pTransform;
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_l");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Player_Dagger";
	XMStoreFloat4x4(&WeaponDesc.PivotMatrix, m_pModel->Get_PivotMatrix());

	CGameObject* pDagger = m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Weapon", L"Prototype_Weapon", &WeaponDesc);
	if (nullptr == pDagger)
		return E_FAIL;

	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_r");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Player_Saber";
	CGameObject* pSaber = m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Weapon", L"Prototype_Weapon", &WeaponDesc);
	if (nullptr == pSaber)
		return E_FAIL;

	return S_OK;
}


CToolAnimObj* CToolAnimObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolAnimObj* pInstance = new CToolAnimObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CToolAnimObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToolAnimObj::Clone(void* pArg)
{
	CToolAnimObj* pInstance = new CToolAnimObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CToolAnimObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolAnimObj::Free()
{
	__super::Free();

	Safe_Release(m_pModel);

	Safe_Release(m_pShader);
}
