#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CVTF final : public CBase
{
private:
	CVTF(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVTF(const CVTF& rhs);
	virtual ~CVTF() = default;

public:
	HRESULT Initialize(void* pArg);

public:
	HRESULT Bind_ShaderResourceView(class CShader* pShader, const char* pConstantName, _uint iMeshIndex);
	void Update_VTF(_float4x4* BoneMatrices, _uint iMeshIdx);

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

	ID3D11Texture2D**			m_Textures = nullptr;
	ID3D11ShaderResourceView**	m_SRVs = nullptr;

private:
	_uint m_iNumMeshes = { 0 };

public:
	static CVTF* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END
