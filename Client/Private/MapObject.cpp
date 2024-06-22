#include "MapObject.h"

CMapObject::CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMapObject::CMapObject(const CMapObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMapObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapObject::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	return S_OK;
}


void CMapObject::Tick(_float fTimeDelta)
{

}

void CMapObject::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CMapObject::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
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



HRESULT CMapObject::Ready_Components(void* pArg)
{
	LOADOBJDESC* pLoadDesc = (LOADOBJDESC*)pArg;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModel"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, pLoadDesc->szModelTag, L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pLoadDesc->WorldMatrix));

	return S_OK;
}


CMapObject* CMapObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapObject* pInstance = new CMapObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMapObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMapObject::Clone(void* pArg)
{
	CMapObject* pInstance = new CMapObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMapObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapObject::Free()
{
	__super::Free();

	Safe_Release(m_pModel);
	Safe_Release(m_pShader);
}
