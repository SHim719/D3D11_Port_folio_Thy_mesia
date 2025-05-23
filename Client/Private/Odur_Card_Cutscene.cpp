#include "Odur_Card_Cutscene.h"

#include "Bone.h"

COdur_Card_Cutscene::COdur_Card_Cutscene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

COdur_Card_Cutscene::COdur_Card_Cutscene(const COdur_Card_Cutscene& rhs)
	: CGameObject(rhs)
{
}

HRESULT COdur_Card_Cutscene::Initialize_Prototype()
{
	return S_OK;
}

HRESULT COdur_Card_Cutscene::Initialize(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_AttachDesc = *(ATTACHDESC*)pArg;

	Safe_AddRef(m_AttachDesc.pAttachBone);
	Safe_AddRef(m_AttachDesc.pParentTransform);
	return S_OK;
}


void COdur_Card_Cutscene::Tick(_float fTimeDelta)
{
	
}

void COdur_Card_Cutscene::LateTick(_float fTimeDelta)
{
	m_pTransform->Attach_To_Bone(m_AttachDesc.pAttachBone, m_AttachDesc.pParentTransform);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_EFFECT_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_GLOW, this);
}

HRESULT COdur_Card_Cutscene::Render()
{
	if (nullptr == m_pModel ||
		nullptr == m_pShader)
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;


	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_DIFFUSE, "g_BaseTexture")))
			return E_FAIL;

		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, 4)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT COdur_Card_Cutscene::Ready_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModel"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Model_Odur_Card"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

COdur_Card_Cutscene* COdur_Card_Cutscene::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	COdur_Card_Cutscene* pInstance = new COdur_Card_Cutscene(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : COdur_Card_Cutscene"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* COdur_Card_Cutscene::Clone(void* pArg)
{
	COdur_Card_Cutscene* pInstance = new COdur_Card_Cutscene(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : COdur_Card_Cutscene"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdur_Card_Cutscene::Free()
{
	__super::Free();

	Safe_Release(m_AttachDesc.pAttachBone);
	Safe_Release(m_AttachDesc.pParentTransform);

	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
}
