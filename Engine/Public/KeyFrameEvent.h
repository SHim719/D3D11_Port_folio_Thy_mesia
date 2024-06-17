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

public:
	void Bind_Func(function<void()> CallBackFunc) { 
		m_CallBackFunc = std::move(CallBackFunc); 
	}
	
	void Execute() {
		if (m_CallBackFunc) m_CallBackFunc();
	}
	
	const string& Get_EventName() const { return m_strEventName; }
														
public:
	static CKeyFrameEvent* Create(void* pArg);
	CKeyFrameEvent* Clone();
	void Free() override;
};

END