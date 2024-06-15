#pragma once

#include "Enemy.h"
#include "Odur_Enums.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class COdur final : public CEnemy
{
private:
	enum ODUR_WEAPON { CANE, SWORD, FOOT, WEAPON_END};

private:
	COdur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COdur(const COdur& rhs);
	virtual ~COdur() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	void Bind_KeyFrames()				override;

private:
	CBone* m_pSwapBone = { nullptr };


private:
	void OnCollisionEnter(CGameObject* pOther)	override;
	void OnCollisionExit(CGameObject* pOther)	override;

private:
	HRESULT Ready_Components();
	HRESULT Ready_States();
	HRESULT Ready_Weapons();

public:
	static COdur* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END