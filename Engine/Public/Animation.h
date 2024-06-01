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
	HRESULT Initialize(ifstream& fin);
	HRESULT Play_Animation(_float fTimeDelta, vector<class CBone*>& Bones);
	HRESULT Play_Animation_Blend(_float fTimeDelta, vector<class CBone*>& Bones);

	void Reset();

private:
	string						m_strAnimName = "";
	_uint						m_iNumChannels = 0;
	vector<class CChannel*>		m_Channels;
	_float						m_fDuration = 0.f;
	_float						m_fTickPerSecond = 0.f;
	_float						m_fPlayTime = 0.f;
	_float						m_fBlendingTime = 0.f;
	_float						m_fPlayRate = 1.f;

private:
	vector<_uint>				m_ChannelKeyFrames;
	vector<_uint>				m_BoneIndices;

public:
	const string& Get_AnimName() { return m_strAnimName; }
	void Set_BlendingTime(_float fTime) { 
		m_fBlendingTime = fTime * m_fTickPerSecond; }

public:
	static CAnimation* Create(ifstream& fin);
	CAnimation* Clone();
	virtual void Free() override;
};

END