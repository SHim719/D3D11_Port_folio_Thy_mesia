#pragma once

#include "Weapon.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class CPlagueWeapon : public CWeapon
{
protected:
	CPlagueWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlagueWeapon(const CPlagueWeapon& rhs);
	virtual ~CPlagueWeapon() = default;

	HRESULT Render()					override;

public:
	static CPlagueWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END