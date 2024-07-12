#include "UI_StunnedMark.h"

#include "Bone.h"

CUI_StunnedMark::CUI_StunnedMark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_StunnedMark::CUI_StunnedMark(const CUI_StunnedMark& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_StunnedMark::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_StunnedMark::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_LockOn"), TEXT("Texture"), (CComponent**)&m_pUITexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	m_pTransform->Set_Scale({ 200.f, 200.f, 1.f });

	return S_OK;
}


void CUI_StunnedMark::Tick(_float fTimeDelta)
{
	_matrix TargetMatrix = m_AttachDesc.pAttachBone->Get_CombinedTransformation() * m_AttachDesc.pParentTransform->Get_WorldMatrix();

	_vector vUIPos = TargetMatrix.r[3];

	m_pTransform->Set_Position(XMVectorSetW(Convert_ScreenToRenderPos(Convert_WorldToScreen(vUIPos)), 1.f));
}

void CUI_StunnedMark::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);

}

HRESULT CUI_StunnedMark::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pUITexture->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(4)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_StunnedMark::OnEnter_Layer(void* pArg)
{
	if (FAILED(__super::OnEnter_Layer(pArg)))
		return E_FAIL;

	Safe_Release(m_AttachDesc.pParentTransform);
	Safe_Release(m_AttachDesc.pAttachBone);

	m_AttachDesc = *(ATTACHDESC*)pArg;

	Safe_AddRef(m_AttachDesc.pParentTransform);
	Safe_AddRef(m_AttachDesc.pAttachBone);

	return S_OK;
}

CUI_StunnedMark* CUI_StunnedMark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_StunnedMark* pInstance = new CUI_StunnedMark(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_StunnedMark"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_StunnedMark::Clone(void* pArg)
{
	CUI_StunnedMark* pInstance = new CUI_StunnedMark(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_StunnedMark"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_StunnedMark::Free()
{
	__super::Free();

	Safe_Release(m_AttachDesc.pParentTransform);
	Safe_Release(m_AttachDesc.pAttachBone);
}
