#include "UI_LockOn.h"

#include "Bone.h"

CUI_LockOn::CUI_LockOn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_LockOn::CUI_LockOn(const CUI_LockOn& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_LockOn::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_LockOn::Initialize(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_LockOn"), TEXT("Texture"), (CComponent**)&m_pUITexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	m_pTransform->Set_Scale({ 100.f, 100.f, 1.f });

	return S_OK;
}

void CUI_LockOn::On_UIActive(void* pArg)
{
	LOCKONDESC* pDesc = (LOCKONDESC*)pArg;

	Safe_Release(m_pTargetTransform);
	Safe_Release(m_pTargetBone);

	m_pTargetTransform = pDesc->pTargetTransform;
	m_pTargetBone = pDesc->pTargetBone;

	Safe_AddRef(m_pTargetTransform);
	Safe_AddRef(m_pTargetBone);
}

void CUI_LockOn::Tick(_float fTimeDelta)
{
	_matrix TargetMatrix = m_pTargetBone->Get_CombinedTransformation() * m_pTargetTransform->Get_WorldMatrix();

	_vector vUIPos = TargetMatrix.r[3];

	_matrix ViewMatrix = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	_matrix ProjMatrix = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);

	vUIPos = XMVector3TransformCoord(vUIPos, ViewMatrix);
	vUIPos = XMVector3TransformCoord(vUIPos, ProjMatrix);
	vUIPos = XMVector3TransformCoord(vUIPos, XMLoadFloat4x4(&m_ViewportMatrix));

	m_pTransform->Set_Position(XMVectorSetW(Convert_ScreenToWorld(vUIPos), 1.f));
}

void CUI_LockOn::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);

}

HRESULT CUI_LockOn::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pUITexture->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}



CUI_LockOn* CUI_LockOn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_LockOn* pInstance = new CUI_LockOn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_LockOn"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_LockOn::Clone(void* pArg)
{
	CUI_LockOn* pInstance = new CUI_LockOn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_LockOn"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_LockOn::Free()
{
	__super::Free();

	Safe_Release(m_pTargetTransform);
	Safe_Release(m_pTargetBone);
}
