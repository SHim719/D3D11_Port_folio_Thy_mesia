#include "UI_Popup.h"

CUI_Popup::CUI_Popup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Popup::CUI_Popup(const CUI_Popup& rhs)
	: CUI(rhs)
{
}


HRESULT CUI_Popup::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pPopupTextures.reserve(2);

	m_pTransform->Set_Scale({ 382.f, 58.f, 1.f });

	XMStoreFloat4(&m_vRenderStartPos, Convert_ScreenToRenderPos(XMVectorSet(1216.f, 304.f, 1.f, 1.f)));

	return S_OK;
}


void CUI_Popup::Tick(_float fTimeDelta)
{
	switch (m_ePopupState)
	{
	case CUI_Popup::Appear:
		m_fAlpha = JoMath::Lerp(0.f, 1.f, 1.f - m_fLerpRate);
		m_fRenderPosX = JoMath::Lerp(m_vRenderStartPos.x - 30.f, m_vRenderStartPos.x, m_fLerpRate);

		m_fLerpRate -= m_fDeltaRateSpeed * fTimeDelta;
		if (m_fLerpRate <= 0.f)
		{
			m_fLerpRate = 0.f;
			m_ePopupState = Wait;
		}
		break;

	case CUI_Popup::Wait:
		m_fTimeAcc += fTimeDelta;
		if (m_fTimeAcc >= m_fWaitTime)
			m_ePopupState = Disappear;
		break;

	case CUI_Popup::Disappear:
		m_fAlpha = JoMath::Lerp(0.f, 1.f, 1.f - m_fLerpRate);
		m_fRenderPosX = JoMath::Lerp(m_vRenderStartPos.x - 30.f, m_vRenderStartPos.x, m_fLerpRate);

		m_fLerpRate += m_fDeltaRateSpeed * fTimeDelta;
		if (m_fLerpRate >= 1.f)
		{
			m_fLerpRate = 1.f;
			m_bReturnToPool = true;
		}

		break;
	}
}

void CUI_Popup::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Popup::Render()
{
	_float4 vRenderPos = m_vRenderStartPos;
	vRenderPos.x = m_fRenderPosX;

	if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	for (size_t i = 0; i < m_pPopupTextures.size(); ++i)
	{
		vRenderPos.y += m_pTransform->Get_Scale().y * i;

		m_pTransform->Set_Position(XMLoadFloat4(&vRenderPos));

		if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pPopupTextures[i]->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(3)))
			return E_FAIL;

		if (FAILED(m_pVIBuffer->Render()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Popup::OnEnter_Layer(void* pArg)
{
	m_bReturnToPool = false;

	vector<SKILLTYPE>* PopupTypes = (vector<SKILLTYPE>*)pArg;

	for (auto& pTexture : m_pPopupTextures)
		Safe_Release(pTexture);

	m_pPopupTextures.resize(PopupTypes->size());

	for (size_t i = 0; i < PopupTypes->size(); ++i)
		m_pPopupTextures[i] = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, Get_PrototypeTag((*PopupTypes)[i])));
	
	m_fRenderPosX = m_vRenderStartPos.x;

	m_ePopupState = Appear;

	m_fAlpha = 0.f;

	m_fLerpRate = 1.f;
	m_fTimeAcc = 0.f;

	PLAY_SOUND(L"Popup", false, 1.f);
	
	return S_OK;
}

HRESULT CUI_Popup::Ready_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;


	return S_OK;
}

wstring CUI_Popup::Get_PrototypeTag(SKILLTYPE ePopupType) const 
{
	wstring wstrTag;

	switch (ePopupType)
	{
	case SKILLTYPE::NONE:
		wstrTag = L"Prototype_Texture_Popup_Key";
		break;
	case SKILLTYPE::AXE:
		wstrTag = L"Prototype_Texture_Popup_Axe";
		break;
	case SKILLTYPE::HAMMER:
		wstrTag = L"Prototype_Texture_Popup_Hammer";
		break;
	case SKILLTYPE::SPEAR:
		wstrTag = L"Prototype_Texture_Popup_Spear";
		break;
	case SKILLTYPE::TWINBLADE:
		wstrTag = L"Prototype_Texture_Popup_TwinBlade";
		break;
	}

	return wstrTag;
}

CUI_Popup* CUI_Popup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Popup* pInstance = new CUI_Popup(pDevice, pContext);

	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Popup"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_Popup::Clone(void* pArg)
{
	CUI_Popup* pInstance = new CUI_Popup(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Popup"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Popup::Free()
{
	__super::Free();

	for (auto& pTexture : m_pPopupTextures)
		Safe_Release(pTexture);

}
