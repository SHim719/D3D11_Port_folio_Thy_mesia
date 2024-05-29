#pragma once


#include "VIBuffer.h"
#include "Model.h"


/* 모델을 구성하는 하나의 메시. */
/* 이 메시를 구성하는 정점, 인덱스 버퍼를 보관한다. */

BEGIN(Engine)

class CMeshContainer final : public CVIBuffer
{
private:
	CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshContainer(const CMeshContainer& rhs);
	virtual ~CMeshContainer() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	HRESULT Initialize(ifstream& fin, CModel::TYPE eModelType);

private:
	char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = 0;

	VTXMODEL*			m_pModelVertices = nullptr;
	VTXANIMMODEL*		m_pAnimVertices = nullptr;
	FACEINDICES32*		m_pIndices = nullptr;
private:
	HRESULT Ready_Vertices(ifstream& fin);
	HRESULT Ready_AnimVertices(ifstream& fin);

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& fin, CModel::TYPE eModelType);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END