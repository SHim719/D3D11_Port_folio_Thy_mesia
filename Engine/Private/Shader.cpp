#include "..\Public\Shader.h"

#include "GameInstance.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_Passes(rhs.m_Passes)
{
	for (auto& PassDesc : m_Passes)
	{
		Safe_AddRef(PassDesc.pInputLayout);
		Safe_AddRef(PassDesc.pPass);
	}


	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, _bool bInRenderer)
{
	_uint		iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else 
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif	

	ID3DBlob* pErrorBlob = nullptr;

	if (FAILED(D3DX11CompileEffectFromFile(strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iFlag, 0, m_pDevice, &m_pEffect, &pErrorBlob)))
	{
		if (pErrorBlob)
		{
			_char* errorMessage = static_cast<_char*>(pErrorBlob->GetBufferPointer());
			MSG_BOX_CStr(errorMessage);
			assert(false);
			pErrorBlob->Release();
		}
		return E_FAIL;
	}


	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC		TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof(D3DX11_TECHNIQUE_DESC));

	if (FAILED(pTechnique->GetDesc(&TechniqueDesc)))
		return E_FAIL;

	for (_uint i = 0; i < TechniqueDesc.Passes; ++i)
	{
		PASSDESC				PassDesc;
		ZeroMemory(&PassDesc, sizeof(PASSDESC));

		PassDesc.pPass = pTechnique->GetPassByIndex(i);

		D3DX11_PASS_DESC		PassInfo;

		PassDesc.pPass->GetDesc(&PassInfo);

		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, PassInfo.pIAInputSignature, PassInfo.IAInputSignatureSize, &PassDesc.pInputLayout)))
			return E_FAIL;

		m_Passes.push_back(PassDesc);
	}

	if (bInRenderer)
		CGameInstance::Get_Instance()->Add_UsingShader(this);

	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CShader::Set_RawValue(const char* pContantName, const void* pData, _uint iByteLength)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	/* Ư�� �̸��� ���� ���̴� ���������� �ڵ��� ���´�. */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pContantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iByteLength);
}

HRESULT CShader::Set_ShaderResourceView(const char* pContantName, ID3D11ShaderResourceView* pSRV)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pContantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pShaderResource = pVariable->AsShaderResource();
	if (nullptr == pShaderResource)
		return E_FAIL;

	if (FAILED(pShaderResource->SetResource(pSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if (iPassIndex >= m_Passes.size())
		return E_FAIL;

	/* �̷��� ������ ���� ������ �׸���. */
	/* �̷��� ������ ����  : �̹� ���̴����� ���� �� ������ �����ߴ�. */
	m_pContext->IASetInputLayout(m_Passes[iPassIndex].pInputLayout);

	/* �ش� ID3DXEffectPass�� �׸���. */
	/* Apply�� ȣ���ϱ����� ��� ������ ���̴��� �����ϴ� ��Ȱ�� �������ؾ��Ѵ�. */
	m_Passes[iPassIndex].pPass->Apply(0, m_pContext);

	return S_OK;
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, _bool bInRenderer)
{
	CShader* pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strShaderFilePath, pElements, iNumElements, bInRenderer)))
	{
		assert(false);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
	CShader* pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		assert(false);
		MSG_BOX(TEXT("Failed To Cloned : CShader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	for (auto& PassDesc : m_Passes)
	{
		Safe_Release(PassDesc.pInputLayout);
	}

	m_Passes.clear();

	Safe_Release(m_pEffect);
}
