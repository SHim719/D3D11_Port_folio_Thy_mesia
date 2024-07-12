#pragma once


#include "VIBuffer.h"
#include "Model.h"


/* ���� �����ϴ� �ϳ��� �޽�. */
/* �� �޽ø� �����ϴ� ����, �ε��� ���۸� �����Ѵ�. */

BEGIN(Engine)

class CMeshContainer final : public CVIBuffer
{
private:
	CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshContainer(const CMeshContainer& rhs);
	virtual ~CMeshContainer() = default;
public:
	HRESULT Initialize(ifstream& fin, CModel::TYPE eModelType);

	_bool Picking(_fmatrix InvWorldMat, _fvector vRayStartPos, _fvector vRayDir, OUT _float4& vPickedPos, OUT _float& fDist)	override;
private:
	VTXMODEL*			m_pModelVertices = nullptr;
	VTXANIMMODEL*		m_pAnimVertices = nullptr;
	FACEINDICES32*		m_pIndices = nullptr;

private:
	HRESULT Ready_Vertices(ifstream& fin);
	HRESULT Ready_AnimVertices(ifstream& fin);
	HRESULT Ready_IndexBuffer(ifstream& fin);

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& fin, CModel::TYPE eModelType);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;


	friend class CMesh_Instance;
};

END