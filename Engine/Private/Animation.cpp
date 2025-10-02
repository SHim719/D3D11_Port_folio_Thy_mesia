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
	, m_ChannelKeyFrames(rhs.m_ChannelKeyFrames)
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);


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

	m_KeyFrameEvents.resize(Get_NumKeyFrames());

	return S_OK;
}

HRESULT CAnimation::Initialize(const CModel::KEYFRAMEEVENTS& Events, const ANIMEVENTS& AnimEvents)
{
	m_KeyFrameEvents = AnimEvents;

	for (size_t i = 0; i < m_KeyFrameEvents.size(); ++i)
	{
		for (CKeyFrameEvent*& KeyFrameEvent : m_KeyFrameEvents[i])
		{
			auto it = Events.find(KeyFrameEvent->Get_EventName());
			if (Events.end() == it)
				return E_FAIL;

			KeyFrameEvent = it->second;
			Safe_AddRef(KeyFrameEvent);
		}
	}
	

	m_bCheckKeyFrames = new _bool[Get_NumKeyFrames()];
	if (nullptr == m_bCheckKeyFrames)
		return E_FAIL;

	ZeroMemory(m_bCheckKeyFrames, Get_NumKeyFrames());

	XMStoreFloat4(&m_vInitialRootPos, Get_RootTransformation().r[3]);

	return S_OK;
}

_bool CAnimation::Play_Animation(_float fTimeDelta, vector<CBone*>& Bones, _bool bLoop, _bool bPlay)
{
	m_fPlayTime += m_fTickPerSecond * fTimeDelta * (_float)bPlay;

	_uint		iChannelIndex = 0;

	for (auto& pChannel : m_Channels)
	{
		m_ChannelKeyFrames[iChannelIndex++] = pChannel->Update_Transformation(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex],
			Bones[m_BoneIndices[iChannelIndex]]);
	}

	Check_KeyFrameEvent();

	if (m_fPlayTime >= m_fDuration)
	{
		m_fPlayTime = m_fDuration;

		if (bLoop)
			Reset();

		return true;
	}
	return false;
}

_bool CAnimation::Play_Animation_Blend(_float fTimeDelta, vector<CBone*>& Bones, _bool bPlay)
{
	m_fPlayTime += m_fTickPerSecond * fTimeDelta * (_float)bPlay;

	m_fBlendingAcc += fTimeDelta;

	_float fRatio = m_fBlendingAcc / m_fBlendingTime;
	if (fRatio > 1.f)
		fRatio = 1.f;
		
	_uint		iChannelIndex = 0;
	for (auto& pChannel : m_Channels)
	{
		m_ChannelKeyFrames[iChannelIndex++] = pChannel->Blend_Transformation(fRatio, m_fPlayTime, m_ChannelKeyFrames[iChannelIndex],
			Bones[m_BoneIndices[iChannelIndex]]);
	}

	Check_KeyFrameEvent();

	return fRatio != 1.f;
}

void CAnimation::Reset()
{
	m_fPlayTime = 0.f;

	m_iPrevKeyFrame = 0;
	m_fBlendingAcc = 0.f;

	for (auto& pChannel : m_Channels)
	{
		for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
			iCurrentKeyFrame = 0;
	}

	memset(m_bCheckKeyFrames, false, Get_NumKeyFrames());
}

_uint CAnimation::Get_NumKeyFrames() const
{
	return m_Channels[0]->Get_NumKeyFrames();
}

void CAnimation::Set_CurrentKeyFrames(_uint iKeyFrame)
{
	if (iKeyFrame >= Get_NumKeyFrames())
		return;

	m_iPrevKeyFrame = iKeyFrame;

	m_fPlayTime = m_Channels[0]->Get_FrameGap() * (_float)iKeyFrame;

	for (auto& pChannel : m_Channels)
	{
		for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
			iCurrentKeyFrame = iKeyFrame;
	}
}

_matrix CAnimation::Get_RootTransformation()
{
	auto it = find_if(m_Channels.begin(), m_Channels.end(), [&](CChannel* pChannel) ->_bool
		{
			return !strcmp(pChannel->Get_Name(), "root");
		});

	if (m_Channels.end() == it)
		return XMMatrixIdentity();

	return (*it)->Get_CurTransformation(m_fPlayTime, Get_NowKeyFrame());
}

void CAnimation::Add_KeyFrameEvent(_int iKeyFrame, CKeyFrameEvent* pEvent)
{
	m_KeyFrameEvents[iKeyFrame].emplace_back(pEvent); 
	Safe_AddRef(pEvent);
}

void CAnimation::Check_KeyFrameEvent()
{
	_int iNowKeyFrame = (_int)Get_NowKeyFrame();
	_int iPrevKeyFrame = m_iPrevKeyFrame;

	// KeyFrame이 한 프레임에 2 프레임이상 띄워질 경우를 대비한 처리
	for (_int i = iPrevKeyFrame; i <= iNowKeyFrame; ++i) 
	{
		if (true == m_bCheckKeyFrames[i])
			continue;
	
		for (CKeyFrameEvent* KeyFrameEvent : m_KeyFrameEvents[i])
		{
			if (KeyFrameEvent)
			{
				KeyFrameEvent->Execute();
			}
		}

		m_bCheckKeyFrames[i] = true;
	}

	m_iPrevKeyFrame = iNowKeyFrame;
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

	for (size_t i = 0; i < m_KeyFrameEvents.size(); ++i)
	{
		for (auto& pKeyFrameEvent : m_KeyFrameEvents[i])
		{
			Safe_Release(pKeyFrameEvent);
		}
	}

	m_KeyFrameEvents.clear();

	Safe_Delete_Array(m_bCheckKeyFrames);
}
