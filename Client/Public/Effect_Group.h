#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CEffect_Group final : public CGameObject
{
private:
	CEffect_Group(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Group(const CEffect_Group& rhs);
	virtual ~CEffect_Group() = default;

public:
	HRESULT Initialize_Prototype(ifstream& fin);
	HRESULT Initialize(void* pArg)					override;
	void Tick(_float fTimeDelta)					override;
	void LateTick(_float fTimeDelta)				override;

	void Check_Destroy();
public:
	HRESULT OnEnter_Layer(void* pArg)			override;
	void Start_Effect(void* pArg);
	void End_Effect();

	_bool Is_Using() const {
		return m_bUsing;
	}

	void Set_Using(_bool bUsing) {
		m_bUsing = bUsing;
	}

	HRESULT Load_EffectGroup(ifstream& fin);
private:
	vector<class CGameEffect*>		m_Effects;
	_bool							m_bUsing = { false };

public:
	static CEffect_Group* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& fin);
	CGameObject* Clone(void* pArg) override;
	void Free() override;
};

END