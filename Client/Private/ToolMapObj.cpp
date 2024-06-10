#include "ToolMapObj.h"



CToolMapObj::CToolMapObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CToolMapObj::CToolMapObj(const CToolMapObj& rhs)
	: CGameObject(rhs)
{
}

HRESULT CToolMapObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CToolMapObj::Initialize(void* pArg)
{
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CToolMapObj::Tick(_float fTimeDelta)
{

}

void CToolMapObj::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CToolMapObj::Render()
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

HRESULT CToolMapObj::Render_Picking(_int iSelectIdx)
{
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

	for (_uint j = 0; j < iNumMeshes; ++j)
	{
		if (FAILED(m_pModel->Render(m_pShader, j, 1)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CToolMapObj::Ready_Components(void* pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModel"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	wstring* strModelPrototypeTag = (wstring*)pArg;

	if (FAILED(__super::Add_Component(LEVEL_TOOL, *strModelPrototypeTag, L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

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
}
