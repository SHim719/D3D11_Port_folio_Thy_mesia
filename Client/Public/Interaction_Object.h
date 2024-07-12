#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CInteraction_Object : public CGameObject
{
protected:
	CInteraction_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteraction_Object(const CInteraction_Object& rhs);
	virtual ~CInteraction_Object() = default;

private:
	

public:
	void Free() override;
};

END

