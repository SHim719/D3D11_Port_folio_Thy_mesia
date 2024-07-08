#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "GameObject.h"
#include "Free_Camera.h"
#include "GameInstance.h"


BEGIN(Client)

class CToolState : public CBase
{
protected:
	CToolState(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CToolState() = default;

public:
	virtual HRESULT Initialize(void* pArg);
	virtual void Start_Tool();
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	CGameInstance*			m_pGameInstance = nullptr;

	class CFree_Camera*		m_pCamera = nullptr;
protected:
	virtual void Camera_Window();
	bool _stdcall VectorOfStringGetter(void* data, _int n, const char** out_text);

public:
	virtual void Free() override;
};

END

