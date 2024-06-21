#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"

BEGIN(Client)
class CFade_Manager
{
	DECLARE_SINGLETON(CFade_Manager)

private:
	CFade_Manager();
	~CFade_Manager() = default;

	void Add_CallBack()

private:
	function<void()>	m_FadeStartCallBack = { nullptr };
	function<void()>	m_FadeEndCallBack = { nullptr };
};

END


