#include "UI_SoulBar.h"

#include "PlayerStats.h"

CUI_SoulBar::CUI_SoulBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_SoulBar::CUI_SoulBar(const CUI_SoulBar& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_SoulBar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	Init_Matrices();

	return S_OK;
}

void CUI_SoulBar::Tick(_float fTimeDelta)
{
	if (m_bSoulUpdated)
	{
		m_fSoulNumber += m_fDeltaSpeed * fTimeDelta;
		if (m_fSoulNumber >= m_fTargetSoulNumber)
		{
			m_fSoulNumber = m_fTargetSoulNumber;
			m_bSoulUpdated = false;
		}
	}
}

HRESULT CUI_SoulBar::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	for (_uint i = 0; i < BAR_END; ++i)
	{
		if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_Matrices[i], sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_Textures[i]->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
			return E_FAIL;
		
		if (FAILED(m_pShader->Begin(3)))
			return E_FAIL;

		if (FAILED(m_pVIBuffer->Render()))
			return E_FAIL;
	}

	return m_pVIBuffer->Bind_Buffers();
}

void CUI_SoulBar::Draw_SoulText()
{
	wstring wstrText = to_wstring((_uint)m_fSoulNumber);
	_float2 vTextRenderPos = m_vFontPos;
	vTextRenderPos.x -= m_fFontScaleX * wstrText.size();

	m_pGameInstance->Render_Font(L"Main_Font", wstrText, vTextRenderPos);

	m_pVIBuffer->Bind_Buffers();
}

void CUI_SoulBar::Init_Matrices()
{
	_matrix WorldMatrix = XMMatrixIdentity();
	_float3 vScale = { 226.f, 25.f, 1.f };

	WorldMatrix.r[0] *= vScale.x;
	WorldMatrix.r[1] *= vScale.y;
	WorldMatrix.r[3] = Convert_ScreenToRenderPos(XMVectorSet(1147.f, 35.f, 1.f, 1.f));

	XMStoreFloat4x4(&m_Matrices[BG], XMMatrixTranspose(WorldMatrix));

	WorldMatrix = XMMatrixIdentity();
	vScale = { 17.f, 25.f, 1.f };

	WorldMatrix.r[0] *= vScale.x;
	WorldMatrix.r[1] *= vScale.y;
	WorldMatrix.r[3] = Convert_ScreenToRenderPos(XMVectorSet(1113.f, 35.f, 1.f, 1.f));

	XMStoreFloat4x4(&m_Matrices[ICON], XMMatrixTranspose(WorldMatrix));
}

void CUI_SoulBar::Set_SoulNumber(_uint iSoulNumber)
{
	m_fTargetSoulNumber = (_float)iSoulNumber;

	m_bSoulUpdated = true;
}

HRESULT CUI_SoulBar::Ready_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_MemoryBG"), TEXT("Texture0"), (CComponent**)&m_Textures[BG])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_MemoryIcon"), TEXT("Texture1"), (CComponent**)&m_Textures[ICON])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;


	return S_OK;
}

CUI_SoulBar* CUI_SoulBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_SoulBar* pInstance = new CUI_SoulBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_SoulBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_SoulBar::Clone(void* pArg)
{
	CUI_SoulBar* pInstance = new CUI_SoulBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_SoulBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_SoulBar::Free()
{
	__super::Free();

	for (_uint i = 0; i < BAR_END; ++i)
		Safe_Release(m_Textures[i]);
}
