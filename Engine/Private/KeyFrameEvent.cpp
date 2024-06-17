#include "KeyFrameEvent.h"


CKeyFrameEvent::CKeyFrameEvent()
{
}

CKeyFrameEvent::CKeyFrameEvent(const CKeyFrameEvent& rhs)
	: m_strEventName(rhs.m_strEventName)
{

}

HRESULT CKeyFrameEvent::Initialize(void* pArg)
{
	m_strEventName = *(string*)pArg;

    return S_OK;
}


CKeyFrameEvent* CKeyFrameEvent::Create(void* pArg)
{
    CKeyFrameEvent* pInstance = new CKeyFrameEvent;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CKeyFrameEvent"));
		Safe_Release(pInstance);
	}

    return pInstance;
}

CKeyFrameEvent* CKeyFrameEvent::Clone()
{
	CKeyFrameEvent* pInstance = new CKeyFrameEvent(*this);

	return pInstance;
}

void CKeyFrameEvent::Free()
{
}
