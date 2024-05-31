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



HRESULT CAnimation::Play_Animation(_float fTimeDelta, vector<CBone*>& Bones)
{
	m_fPlayTime += m_fTickPerSecond * fTimeDelta;

	if (m_fPlayTime >= m_fDuration)
		Reset();

	_uint		iChannelIndex = 0;

	for (auto& pChannel : m_Channels)
	{
		m_ChannelKeyFrames[iChannelIndex++] = pChannel->Update_Transformation(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex],
			Bones[m_BoneIndices[iChannelIndex]]);
	}
	

	return S_OK;
}

HRESULT CAnimation::Play_Animation_Blend(_float fTimeDelta, vector<CBone*>& Bones)
{
	m_fPlayTime += m_fTickPerSecond * fTimeDelta;

	_float fRatio = m_fPlayTime / m_fBlendingTime;
	if (fRatio > 1.f)
		return E_FAIL;

	_uint		iChannelIndex = 0;
	for (auto& pChannel : m_Channels)
	{
		m_ChannelKeyFrames[iChannelIndex++] = pChannel->Blend_Transformation(fRatio, m_fPlayTime, m_ChannelKeyFrames[iChannelIndex],
			Bones[m_BoneIndices[iChannelIndex]]);
	}

	return S_OK;
}



void CAnimation::Reset()
{
	m_fPlayTime = 0.f;

	for (auto& pChannel : m_Channels)
	{
		for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
			iCurrentKeyFrame = 0;
	}
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
