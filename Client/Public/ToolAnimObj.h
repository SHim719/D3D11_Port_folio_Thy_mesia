#pragma once


#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Client)
class CToolAnimObj final : public CGameObject
{
public:
	enum PrototypeCharacter { Corvus, Odur, Villager_F, Villager_M, Joker, HalberdKnight, TwinBladeKnight };

private:
	CToolAnimObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolAnimObj(const CToolAnimObj& rhs);
	virtual ~CToolAnimObj() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	CShader*			m_pShader = nullptr;
	CModel*				m_pModel = nullptr;

private:
	HRESULT Ready_Corvus();
	HRESULT Ready_Odur();
	HRESULT Ready_Villager_F();
	HRESULT Ready_Villager_M();
	HRESULT Ready_Joker();
	HRESULT Ready_HalberdKnight();
	HRESULT Ready_TwinBladeKnight();

public:
	CModel* Get_Model() const { return m_pModel; }

public:
	static CToolAnimObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END

