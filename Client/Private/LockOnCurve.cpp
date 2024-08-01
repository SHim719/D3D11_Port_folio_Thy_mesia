#include "LockOnCurve.h"


CLockOnCurve::CLockOnCurve(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CLockOnCurve::CLockOnCurve(const CLockOnCurve& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLockOnCurve::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLockOnCurve::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CLockOnCurve::Tick(_float fTimeDelta)
{
	m_vMaskUVOffset.x -= fTimeDelta;
	if (m_vMaskUVOffset.x <= -1.f)
		m_vMaskUVOffset.x = 1.f;
}

void CLockOnCurve::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_EFFECT_NONBLEND, this);
}

HRESULT CLockOnCurve::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_PlayerWorldMatrix", &m_tCurveDescs.PlayerWorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vTargetPosition", &m_tCurveDescs.vTargetPos, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vMaskUVOffset", &m_vMaskUVOffset, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pMaskTexture->Set_SRV(m_pShader, "g_MaskTexture", 24)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}


HRESULT CLockOnCurve::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_LockOnCurve"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Masking"), TEXT("Texture"), (CComponent**)&m_pMaskTexture)))
		return E_FAIL;

	return S_OK;
}

CLockOnCurve* CLockOnCurve::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLockOnCurve* pInstance = new CLockOnCurve(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CLockOnCurve"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLockOnCurve::Clone(void* pArg)
{
	CLockOnCurve* pInstance = new CLockOnCurve(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CLockOnCurve"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLockOnCurve::Free()
{
	__super::Free();

	Safe_Release(m_pMaskTexture);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
}
