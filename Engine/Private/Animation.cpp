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
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}


HRESULT CAnimation::Initialize(aiAnimation* pAIAnimation)
{
	m_strAnimName = pAIAnimation->mName.C_Str();

	m_fDuration = (_float)pAIAnimation->mDuration;
	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	/* ���� �ִϸ��̼ǿ��� �����ؾ��� ������ ������ �����Ѵ�. */
	m_iNumChannels = pAIAnimation->mNumChannels;


	/* ���� �ִϸ��̼ǿ��� �����ؾ��� ���������� �����Ͽ� �����Ѵ�. */
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		/* aiNodeAnim : mChannel�� Ű������ �������� ������. */
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		/* �� ��Ƶδµ�?> Ư�� �ִԿ��Ӽ� �����϶� �ִϸ��̼��� ����ϸ� ��� ���� ���¸� �����ϴ°� ����. ����.
		���� �ֹ̿��ÿ��� �����ϱ����� ���븸 ���� �������ֱ� ����. */
		m_Channels.push_back(pChannel);
	}


	for (_uint i = 0; i < m_iNumChannels; ++i)
		m_ChannelKeyFrames.push_back(0);
	

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

	/* �� �ִϸ��̼� ������ ���� ��� ������ ��ȸ�ϸ� ������ ����� �������ش�. */
	/* Transformation : ���޵� �ð��� ���� Ű������(�ð�, ������, ȸ��, �̵�)������ �̿��Ͽ� Transformation�� �����. */
	/* ���̾��Ű ��忡 �������ش�. */
	for (auto& pChannel : m_Channels)
	{
		m_ChannelKeyFrames[iChannelIndex] = pChannel->Update_Transformation(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex], Bones[iChannelIndex]);

		++iChannelIndex;
	}

	return S_OK;
}


CAnimation* CAnimation::Create(aiAnimation* pAIAnimation)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();

}
