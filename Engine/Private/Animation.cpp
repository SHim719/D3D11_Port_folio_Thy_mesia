#include "..\Public\Animation.h"
#include "Channel.h"

#include "Bone.h"

#include "KeyFrameEvent.h"

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


HRESULT CAnimation::Initialize_Prototype(ifstream& fin)
{
	_uint iNameLen = 0;
	char szAnimName[MAX_PATH] = "";
	fin.read((char*)&iNameLen, sizeof(_uint));
	fin.read(szAnimName, iNameLen);

	m_strAnimName = szAnimName;

	fin.read((char*)&m_fDuration, sizeof(_float));
	fin.read((char*)&m_fTickPerSecond, sizeof(_float));
	fin.read((char*)&m_iNumChannels, sizeof(_uint));
	fin.read((char*)&m_fPlayRate, sizeof(_float));

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

HRESULT CAnimation::Initialize(const CModel::KEYFRAMEEVENTS& Events, const ANIMEVENTS& AnimEvents)
{
	for (auto& Pair : AnimEvents)
		m_KeyFrameEvents.emplace(Pair);
	

	for (auto& Pair : m_KeyFrameEvents)
	{
		auto it = Events.find(Pair.second->Get_EventName());
		if (Events.end() == it)
			return E_FAIL;
		
		Pair.second = it->second;
		Safe_AddRef(Pair.second);
	}


	return S_OK;
}



_bool CAnimation::Play_Animation(_float fTimeDelta, vector<CBone*>& Bones, _bool bPlay)
{
	Check_KeyFrameEvent();

	m_fPlayTime += m_fTickPerSecond * fTimeDelta * (_float)bPlay;

	if (m_fPlayTime >= m_fDuration)
	{
		Reset();
		return true;
	}
	
	_uint		iChannelIndex = 0;

	for (auto& pChannel : m_Channels)
	{
		m_ChannelKeyFrames[iChannelIndex++] = pChannel->Update_Transformation(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex],
			Bones[m_BoneIndices[iChannelIndex]]);
	}

	return false;
}

_bool CAnimation::Play_Animation_Blend(_float fTimeDelta, vector<CBone*>& Bones, _bool bPlay)
{
	Check_KeyFrameEvent();

	m_fPlayTime += m_fTickPerSecond * fTimeDelta * (_float)bPlay;

	_float fRatio = m_fPlayTime / m_fBlendingTime;
	if (fRatio > 1.f)
		return false;

	_uint		iChannelIndex = 0;
	for (auto& pChannel : m_Channels)
	{
		m_ChannelKeyFrames[iChannelIndex++] = pChannel->Blend_Transformation(fRatio, m_fPlayTime, m_ChannelKeyFrames[iChannelIndex],
			Bones[m_BoneIndices[iChannelIndex]]);
	}

	return true;
}



void CAnimation::Reset()
{
	m_fPlayTime = 0.f;

	for (auto& pChannel : m_Channels)
	{
		for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
			iCurrentKeyFrame = 0;
	}

	Reset_KeyFrameEvents();
}

_uint CAnimation::Get_NumKeyFrames() const
{
	return m_Channels[0]->Get_NumKeyFrames();
}

void CAnimation::Set_CurrentKeyFrames(_uint iKeyFrame)
{
	if (iKeyFrame >= Get_NumKeyFrames())
		return;

	m_fPlayTime = m_Channels[0]->Get_FrameGap() * (_float)iKeyFrame;

	for (auto& pChannel : m_Channels)
	{
		for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
			iCurrentKeyFrame = iKeyFrame;
	}
}

void CAnimation::Add_KeyFrameEvent(_int iKeyFrame, CKeyFrameEvent* pEvent)
{
	m_KeyFrameEvents.insert({ iKeyFrame, pEvent }); 
	Safe_AddRef(pEvent);
}

void CAnimation::Reset_KeyFrameEvents()
{
	for (auto it = m_KeyFrameEvents.begin(); it != m_KeyFrameEvents.end(); ++it)
		it->second->Reset();
}

void CAnimation::Check_KeyFrameEvent()
{
	_int iNowKeyFrame = (_int)Get_NowKeyFrame();
	auto Pair = m_KeyFrameEvents.equal_range(iNowKeyFrame);
	if (m_KeyFrameEvents.end() != Pair.first)
	{
		for (auto it = Pair.first; it != Pair.second; ++it)
			it->second->Execute(iNowKeyFrame);
	}
}

CAnimation* CAnimation::Create(ifstream& fin)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize_Prototype(fin)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone(const CModel::KEYFRAMEEVENTS& Events)
{
	CAnimation* pInstance = new CAnimation(*this);

	if (FAILED(pInstance->Initialize(Events, m_KeyFrameEvents)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();

	for (auto& Pair : m_KeyFrameEvents)
		Safe_Release(Pair.second);

	m_KeyFrameEvents.clear();
}
