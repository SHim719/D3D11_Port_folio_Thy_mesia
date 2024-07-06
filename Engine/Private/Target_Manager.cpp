#include "..\Public\Target_Manager.h"
#include "RenderTarget.h"

#include "Shader.h"
#include "VIBuffer_Rect.h"

CTarget_Manager::CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CTarget_Manager::Initialize()
{


	return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(const wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixel, const _float4& vClearColor)
{
	if (nullptr != Find_RenderTarget(strRenderTargetTag))
		return E_FAIL;

	CRenderTarget::RT_DESC RT_Desc;
	ZeroMemory(&RT_Desc, sizeof(CRenderTarget::RT_DESC));
	RT_Desc.iWidth = iWidth;
	RT_Desc.iHeight = iHeight;
	RT_Desc.eFormat = ePixel;
	RT_Desc.vClearColor = vClearColor;

	CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, &RT_Desc);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(strRenderTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const wstring& strMRTTag, const wstring& strRenderTargetTag)
{
	/* ��Ƽ ���� Ÿ�ٿ� �߰����ֱ����� ���� Ÿ���� ã�Ҵ�. */
	CRenderTarget* pRenderTarget = Find_RenderTarget(strRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	/* ������ ã�Ƴ��� ����Ÿ���� �߰����ֱ����� ��Ƽ���� Ÿ���� ã��. */
	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

	/* �߰��Ϸ� �ߴ� ��Ƽ���� Ÿ�� �׷��� ������. */
	/* ���Ӱ� �׷��� ����������. */
	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>	MRTList;
		MRTList.emplace_back(pRenderTarget);

		m_MRTs.emplace(strMRTTag, MRTList);
	}
	else 	/* �߰��Ϸ� �ߴ� ��Ƽ���� Ÿ�� �׷��� �־���. */
		pMRTList->emplace_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(const wstring& strMRTTag)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	/* ��ġ�� �̹� ���εǾ��־��� ����Ÿ�� �ϳ��� �����ð�.(1��, �����) */
	/* ���� ���۵� �����ͼ� �����ص���. */
	m_pContext->OMGetRenderTargets(1, &m_pOldRTV, &m_pDSV);

	ID3D11RenderTargetView* pMRTs[8] = {};

	_uint		iNumRenderTargets = { 0 };

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pMRTs[iNumRenderTargets++] = pRenderTarget->Get_RTV();
	}

	/* ��ġ�� ����Ÿ���� ���ε��Ѵ�. */
	/* == �� ����Ÿ�ٿ� ������ �׸��ڴ�. */
	/* == �׸��� ���� ���� �׽�Ʈ�� �ʿ��ϴ�. */
	/* == ����Ÿ�ٰ� ���̹��۸� �Բ� ���ε�����.  */
	m_pContext->OMSetRenderTargets(iNumRenderTargets, pMRTs, m_pDSV);

	return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
	m_pContext->OMSetRenderTargets(1, &m_pOldRTV, m_pDSV);

	Safe_Release(m_pOldRTV);
	Safe_Release(m_pDSV);

	return S_OK;
}

HRESULT CTarget_Manager::Bind_RT_SRV(const wstring& strRenderTargetTag, CShader* pShader, const _char* pConstantName)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_ShaderResourceView(pShader, pConstantName);
}

#ifdef _DEBUG

HRESULT CTarget_Manager::Ready_Debug(const wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CTarget_Manager::Render_Debug(const wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	if (FAILED(pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
		pRenderTarget->Render_Debug(pShader, pVIBuffer);

	return S_OK;
}

#endif

CRenderTarget* CTarget_Manager::Find_RenderTarget(const wstring& strRenderTargetTag)
{
	auto	iter = m_RenderTargets.find(strRenderTargetTag);

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const wstring& strMRTTag)
{
	auto	iter = m_MRTs.find(strMRTTag);

	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

CTarget_Manager* CTarget_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTarget_Manager* pInstance = new CTarget_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CTarget_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTarget_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);

		Pair.second.clear();
	}
	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);

	m_RenderTargets.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
