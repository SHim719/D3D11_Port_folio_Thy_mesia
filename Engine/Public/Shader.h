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
	/* 셰이더 전역변수에 데이터를 던진다. */
	HRESULT Set_RawValue(const char* pContantName, const void* pData, _uint iByteLength);
	HRESULT Set_ShaderResourceView(const char* pContantName, ID3D11ShaderResourceView* pSRV);

public: /* 이 셰이더의 특정 패스로 그린다. */
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