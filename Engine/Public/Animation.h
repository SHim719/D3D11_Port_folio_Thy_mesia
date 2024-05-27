#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(aiAnimation* pAIAnimation);
	HRESULT Play_Animation(_float fTimeDelta, vector<class CBone*>& Bones, _bool bLoop);

	void Reset() {
		m_fPlayTime = 0.f;
	};

private:
	string						m_strAnimName = "";

	/* �� �ִϸ��̼��� �����ϱ����� ���Ǵ� ���� ����. */
	_uint						m_iNumChannels = 0;
	vector<class CChannel*>		m_Channels;

	/* �ִϸ��̼� ����ϴµ� �ɸ��� ��ü�ð�. */
	_float						m_fDuration = 0.f;

	/* �ִϸ��̼��� �ʴ� ��� �ӵ�. */
	_float						m_fTickPerSecond = 0.f;

	_float						m_fPlayTime = 0.f;

private:
	vector<_uint>				m_ChannelKeyFrames;

public:
	const string& Get_AnimName() { return m_strAnimName; }

public:
	static CAnimation* Create(aiAnimation* pAIAnimation);
	virtual void Free() override;
};

END