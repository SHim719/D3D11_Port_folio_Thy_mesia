#include "UI_DamageFont.h"

CUI_DamageFont::CUI_DamageFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{

}

CUI_DamageFont::CUI_DamageFont(const CUI_DamageFont& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_DamageFont::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CUI_DamageFont::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransform->Set_Scale({ 40.f, 40.f, 1.f });

	return S_OK;
}


void CUI_DamageFont::Tick(_float fTimeDelta)
{
	m_fAlpha -= m_fDeltaAlphaSpeed * fTimeDelta;
	if (m_fAlpha < 0.f)
	{
		m_fAlpha = 0.f;
		m_bReturnToPool = true;
	}

	m_vRenderStartPos.y += m_fFontMoveSpeed * fTimeDelta;
}

void CUI_DamageFont::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);

}

HRESULT CUI_DamageFont::Render()
{
	_vector vRenderPos = XMLoadFloat4(&m_vRenderStartPos);

	for (size_t i = 0; i < m_strDamage.size(); ++i)
	{
		m_pTransform->Set_Position(vRenderPos);

		if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pUITexture->Set_SRV(m_pShader, "g_DiffuseTexture", m_strDamage[i] - '0')))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(3)))
			return E_FAIL;

		if (FAILED(m_pVIBuffer->Render()))
			return E_FAIL;

		vRenderPos.m128_f32[0] += m_fFontGapX;
	}

	return S_OK;
}

HRESULT CUI_DamageFont::OnEnter_Layer(void* pArg)
{
	DAMAGEFONTDESC* pDesc = (DAMAGEFONTDESC*)pArg;
	
	m_strDamage = pDesc->strDamage;

	_vector vScreenPosition = XMLoadFloat4(&pDesc->vWorldPosition);

	_matrix ViewMatrix = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	_matrix ProjMatrix = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);

	vScreenPosition = XMVector3TransformCoord(vScreenPosition, ViewMatrix);
	vScreenPosition = XMVector3TransformCoord(vScreenPosition, ProjMatrix);
	vScreenPosition = XMVector3TransformCoord(vScreenPosition, XMLoadFloat4x4(&m_ViewportMatrix));

	vScreenPosition.m128_f32[0] += (_float)JoRandom::Random_Int(-20, 20);
	vScreenPosition.m128_f32[1] -= 20.f;

	vScreenPosition.m128_f32[0] -= m_fFontGapX * _float(m_strDamage.size() >> 1);

	XMStoreFloat4(&m_vRenderStartPos, Convert_ScreenToRenderPos(vScreenPosition));

	m_fAlpha = 1.f;

	return S_OK;
}


HRESULT CUI_DamageFont::Ready_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_DamageFont"), TEXT("Texture"), (CComponent**)&m_pUITexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

CUI_DamageFont* CUI_DamageFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_DamageFont* pInstance = new CUI_DamageFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_DamageFont"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_DamageFont::Clone(void* pArg)
{
	CUI_DamageFont* pInstance = new CUI_DamageFont(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_DamageFont"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_DamageFont::Free()
{
	__super::Free();
}
