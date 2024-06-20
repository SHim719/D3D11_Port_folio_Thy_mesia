#pragma once

#include "Client_Defines.h"
#include "Level.h"


BEGIN(Client)

class CLevel_Tool final : public CLevel
{
private:
	CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Tool() = default;

public:
	HRESULT Initialize()			override;
	void Tick(_float fTimeDelta)	override;
	HRESULT Render()				override;

private:
	HRESULT Ready_ToolObjects();
	HRESULT Ready_Camera();
	HRESULT Ready_Player();
	HRESULT Ready_Odur();

	HRESULT Ready_UIResource();
	HRESULT Ready_UI();

	HRESULT Ready_Navigation();

public:
	static CLevel_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END


