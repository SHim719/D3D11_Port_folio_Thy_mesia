#pragma once

#include "Base.h"

BEGIN(Engine)

class CKeyFrameEvent final : public CBase
{
private:
	CKeyFrameEvent();
	CKeyFrameEvent(const CKeyFrameEvent& rhs);
	virtual ~CKeyFrameEvent() = default;

	HRESULT Initialize(void* pArg);

private:
	string				m_strEventName = "";
	function<void()>	m_CallBackFunc = { nullptr };
	_bool				m_bCalled = { false };

	_int				m_iLastKeyFrame = { -1 };
public:
	void Bind_Func(function<void()> CallBackFunc) { 
		m_CallBackFunc = std::move(CallBackFunc); 
	}
	
	void Reset() {
		m_iLastKeyFrame = -1;
	}

	void Execute(_int iNowKeyFrame);
	
	const string& Get_EventName() const { return m_strEventName; }
														
public:
	static CKeyFrameEvent* Create(void* pArg);
	CKeyFrameEvent* Clone();
	void Free() override;
};

END