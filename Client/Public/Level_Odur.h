#pragma once

#include "Client_Defines.h"
#include "ClientLevel.h"

BEGIN(Client)

class CLevel_Odur final : public CClientLevel
{
private:
	CLevel_Odur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Odur() = default;

public:
	HRESULT Initialize()			override;
	void Tick(_float fTimeDelta)	override;
	HRESULT Render()				override;

public:
	static CLevel_Odur* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free() override;
};

END


