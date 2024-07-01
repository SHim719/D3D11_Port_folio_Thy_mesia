#pragma once

#include "Client_Defines.h"
#include "Level.h"


BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	HRESULT Initialize()			override;
	void Tick(_float fTimeDelta)	override;
	HRESULT Render()				override;

private:
	HRESULT Ready_GameObjects();

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END


