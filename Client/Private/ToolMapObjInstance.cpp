#include "ToolMapObjInstance.h"
#include "Mesh_Instance.h"

CToolMapObjInstance::CToolMapObjInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CToolMapObjInstance::CToolMapObjInstance(const CToolMapObjInstance& rhs)
	: CGameObject(rhs)
{

}

HRESULT CToolMapObjInstance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CToolMapObjInstance::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	wstring* pStrModelTag = (wstring*)pArg;

	*pStrModelTag += L"_Instance";

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModelInstance"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_TOOL, *pStrModelTag, L"Model", (CComponent**)&m_pModel_Instance)))
		return E_FAIL;

	return S_OK;
}

void CToolMapObjInstance::Ready_RenderInstance()
{
	m_pModel_Instance->Ready_Instancing(m_pInstance_Transforms);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

	m_pInstance_Transforms.clear();
}

HRESULT CToolMapObjInstance::Render()
{
	_uint		iNumMeshes = m_pModel_Instance->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel_Instance->SetUp_OnShader(m_pShader, i, TextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		/*if (FAILED(m_pModel_Instance->SetUp_OnShader(m_pShaderCom, m_pModel->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/

		if (FAILED(m_pModel_Instance->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel_Instance->Render(m_pShader, i, 0)))
			return E_FAIL;
	}


	return S_OK;
}



CToolMapObjInstance* CToolMapObjInstance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolMapObjInstance* pInstance = new CToolMapObjInstance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CToolMapObjInstance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToolMapObjInstance::Clone(void* pArg)
{
	CToolMapObjInstance* pInstance = new CToolMapObjInstance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CToolMapObjInstance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolMapObjInstance::Free()
{
	__super::Free();

	Safe_Release(m_pModel_Instance);
	Safe_Release(m_pShader);
}
