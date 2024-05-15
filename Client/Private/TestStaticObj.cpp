#include "TestStaticObj.h"


CTestStaticObj::CTestStaticObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTestStaticObj::CTestStaticObj(const CTestStaticObj& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTestStaticObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTestStaticObj::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CTestStaticObj::Tick(_float fTimeDelta)
{
}

void CTestStaticObj::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CTestStaticObj::Render()
{
	if (nullptr == m_pModel ||
		nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, m_pModel->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModel->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/


		if (FAILED(m_pModel->Render(m_pShader, i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CTestStaticObj::Ready_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModel"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_BlackBoard"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

CTestStaticObj* CTestStaticObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTestStaticObj* pInstance = new CTestStaticObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTestStaticObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTestStaticObj::Clone(void* pArg)
{
	CTestStaticObj* pInstance = new CTestStaticObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTestStaticObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestStaticObj::Free()
{
	__super::Free();

	Safe_Release(m_pModel);
	Safe_Release(m_pShader);
}
