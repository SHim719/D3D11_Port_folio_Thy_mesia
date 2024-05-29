#pragma once

#include "ToolState.h"

BEGIN(Client)

class CTestPlay_Tool : public CToolState
{
protected:
	CTestPlay_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTestPlay_Tool() = default;

public:
	HRESULT Initialize(void* pArg)	override;
	void Start_Tool()				override;
	void Tick(_float fTimeDelta)	override;

private:
	void Main_Window();
	void Camera_Window() override;

private:
	class CMain_Camera* m_pMain_Camera = nullptr;
	class CPlayer*		m_pPlayer = nullptr;

public:
	static CTestPlay_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END

