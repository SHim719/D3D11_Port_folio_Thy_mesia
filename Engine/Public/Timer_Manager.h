#pragma once

#include "Base.h"

/* �������� Ÿ�̸Ӹ� �±�(Key)�� �����Ͽ� �����Ѵ�. */
/* Ư�� Ÿ�̸Ӹ� ����ϰ� �������ش�. */

BEGIN(Engine)

class CTimer_Manager final : public CBase
{
private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	_float	Get_TimeDelta(const wstring& strTimerTag);
	void    Set_TimeDivide(_float fTimeDivide);
public:
	HRESULT Add_Timer(const wstring& strTimerTag);
	void Compute_TimeDelta(const wstring& strTimerTag);

private:
	map<const wstring, class CTimer*>			m_Timers;

private:
	class CTimer* Find_Timer(const wstring& strTimerTag);

public:
	static CTimer_Manager* Create();
	virtual void Free() override;
};

END