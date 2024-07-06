#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
public:
	typedef struct tagPassDesc
	{
		ID3DX11EffectPass* pPass = nullptr;
		ID3D11InputLayout* pInputLayout = nullptr;
	}PASSDESC;
private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	HRESULT Initialize_Prototype(const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, _bool bInRenderer);
	HRESULT Initialize(void* pArg)	override;

public:
	/* ���̴� ���������� �����͸� ������. */
	HRESULT Set_RawValue(const char* pContantName, const void* pData, _uint iByteLength);
	HRESULT Set_ShaderResourceView(const char* pContantName, ID3D11ShaderResourceView* pSRV);

public: /* �� ���̴��� Ư�� �н��� �׸���. */
	HRESULT Begin(_uint iPassIndex);

private:
	ID3DX11Effect*				m_pEffect = { nullptr };
	vector<PASSDESC>			m_Passes;

public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, _bool bInRenderer = true);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END