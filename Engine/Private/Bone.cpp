#include "Bone.h"
#include "Model.h"

CBone::CBone()
{

}

CBone::CBone(const CBone& rhs)
	: m_OffsetMatrix(rhs.m_OffsetMatrix)
	, m_Transformation(rhs.m_Transformation)
	, m_iDepth(rhs.m_iDepth)
	, m_iParentBoneIdx(rhs.m_iParentBoneIdx)
{
	strcpy_s(m_szName, rhs.m_szName);
	XMStoreFloat4x4(&m_CombinedTransformation, XMMatrixIdentity());
}


HRESULT CBone::Initialize(ifstream& fin, CModel* pModel)
{
	fin.read(m_szName, sizeof(m_szName));
	fin.read((char*)&m_OffsetMatrix, sizeof(_float4x4));
	fin.read((char*)&m_Transformation, sizeof(_float4x4));
	fin.read((char*)&m_iDepth, sizeof(_uint));
	fin.read((char*)&m_iParentBoneIdx, sizeof(_int));

	return S_OK;
}

void CBone::Set_CombinedTransformation(const vector<CBone*>& Bones)
{
	if (m_iParentBoneIdx >= 0)
		XMStoreFloat4x4(&m_CombinedTransformation, XMLoadFloat4x4(&m_Transformation) * XMLoadFloat4x4(&Bones[m_iParentBoneIdx]->m_CombinedTransformation));
	else
		m_CombinedTransformation = m_Transformation;
}

void CBone::Set_OffsetMatrix(_fmatrix OffsetMatrix)
{
	XMStoreFloat4x4(&m_OffsetMatrix, OffsetMatrix);
}

CBone* CBone::Create(ifstream& fin, CModel* pModel)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(fin, pModel)))
	{
		MSG_BOX(TEXT("Failed To Created : CBone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	CBone* pInstance = new CBone(*this);

	return pInstance;
}

void CBone::Free()
{
}
