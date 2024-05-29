#include "..\Public\Animation.h"
#include "Channel.h"
#include "Model.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_fDuration(rhs.m_fDuration)
	, m_Channels(rhs.m_Channels)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_fPlayTime(rhs.m_fPlayTime)
	, m_strAnimName(rhs.m_strAnimName)
	, m_BoneIndices(rhs.m_BoneIndices)
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);

	m_ChannelKeyFrames.resize(rhs.m_ChannelKeyFrames.size(), 0);
}

HRESULT CAnimation::Initialize(ifstream& fin)
{
	_uint iNameLen = 0;
	char szAnimName[MAX_PATH] = "";
	fin.read((char*)&iNameLen, sizeof(_uint));
	fin.read(szAnimName, iNameLen);

	m_strAnimName = szAnimName;

	fin.read((char*)&m_fDuration, sizeof(_float));
	fin.read((char*)&m_fTickPerSecond, sizeof(_float));
	fin.read((char*)&m_iNumChannels, sizeof(_uint));

	m_BoneIndices.reserve(m_iNumChannels);
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		_uint iBoneIdx = 0;
		fin.read((char*)&iBoneIdx, sizeof(_uint));
		m_BoneIndices.push_back(iBoneIdx);
	}

	m_Channels.reserve(m_iNumChannels);
	m_ChannelKeyFrames.reserve(m_iNumChannels);
	for (size_t i = 0; i < m_iNumChannels; ++i)
	{
		m_ChannelKeyFrames.push_back(0);

		CChannel* pChannel = CChannel::Create(fin);
		if (nullptr == pChannel)
			return E_FAIL;
		m_Channels.push_back(pChannel);
	}

	return S_OK;
}


HRESULT CAnimation::Play_Animation(_float fTimeDelta, vector<CBone*>& Bones, _bool bLoop)
{
	m_fPlayTime += m_fTickPerSecond * fTimeDelta;

	if (m_fPlayTime >= m_fDuration)
	{
		m_fPlayTime = 0.f;

		for (auto& pChannel : m_Channels)
		{
			for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
				iCurrentKeyFrame = 0;
		}
	}

	_uint		iChannelIndex = 0;

	/* 이 애니메이션 구동을 위한 모든 뼈들을 순회하며 뼈들의 행렬을 갱신해준다. */
	/* Transformation : 전달된 시간에 따른 키프레임(시간, 스케일, 회전, 이동)정보를 이용하여 Transformation을 만든다. */
	/* 하이어라키 노드에 저장해준다. */
	for (auto& pChannel : m_Channels)
	{
		m_ChannelKeyFrames[iChannelIndex] = pChannel->Update_Transformation(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex],
			Bones[m_BoneIndices[iChannelIndex]]);

		++iChannelIndex;
	}

	return S_OK;
}


CAnimation* CAnimation::Create(ifstream& fin)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(fin)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	CAnimation* pInstance = new CAnimation(*this);

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();

}
